#include "stdafx.h"
#include "TSReader.h"

namespace Decode
{
	static BOOL IsCRCValid(const BYTE* bits, INT size)
	{
		UINT32 crc = 0xffffffffu;
		const UINT32 CrcPoly = 0x4c11db7;
		for (INT k = 0; k < size; k++)
		{
			INT s = 8;
			UINT32 value = bits[k];
			value <<= (32 - s);
			while (s > 0)
			{
				if ((value ^ crc) & 0x80000000)
				{
					crc <<= 1;
					crc ^= CrcPoly;
				}
				else
				{
					crc <<= 1;
				}
				value <<= 1;
				s--;
			}
		}
		return (crc == 0);
	}
	const BYTE PROGRAM_STREAM_MAP = 0xBC;
	const BYTE PADDING_STREAM = 0xBE;
	const BYTE PRIVATE_STREAM_2 = 0xBF;
	const BYTE ECM_STREAM = 0xF0;
	const BYTE EMM_STREAM = 0xF1;
	const BYTE PROGRAM_STREAM_DIRECTORY = 0xFF;
	const BYTE DSMCC_STREAM = 0xF2;
	const BYTE H222_TYPE_E = 0xF8;
	const BYTE E_STREAM = 0xF8;
	//////////////////////////////////////////////////////////////////////////
	BOOL TSH264Parser::Parse(const BYTE* bits, LONG size)
	{

		return TRUE;
	}
	BOOL TSAACParser::Parse(const BYTE* bits, LONG size)
	{
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	GrowableIOBuffer& TSParser::GetIO()
	{
		return m_io;
	}
	TS_PACKET_STREAM::TS_PACKET_STREAM()
	{

	}
	TS_PACKET_STREAM::~TS_PACKET_STREAM()
	{

	}
	TSParser* TS_PACKET_STREAM::GetParser()
	{
		if (!m_parser)
		{
			if (StreamType == TS_STREAM_TYPE_AUDIO_AAC)
				m_parser.Reset(new TSAACParser());
			if (StreamType == TS_STREAM_TYPE_VIDEO_H264)
				m_parser.Reset(new TSH264Parser());
		}
		return m_parser;
	}
	BOOL TS_PACKET_STREAM::operator == (const TS_PACKET_STREAM& other)
	{
		return ElementaryPID == other.ElementaryPID;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	TSReader::TSReader()
		:m_versionNumber(-1),
		m_continuityCounter(-1)
	{
		ZeroMemory(m_bits, TS_PACKET_SIZE);
	}

	TSReader::~TSReader()
	{
	}
	BOOL TSReader::OpenFile(LPCSTR pzFile)
	{
		if (!pzFile)
			return FALSE;
		this->Close();
		HRESULT hRes = SHCreateStreamOnFileA(pzFile, STGM_READ | STGM_FAILIFTHERE, &m_stream);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL TSReader::ReadBlock(TS_BLOCK& block)
	{
		for (;;)
		{
			TS_PACKEG_HEADER header;
			if (!ReadPacket(header, block))
				return FALSE;
		}
	}
	INT TSReader::ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, const BYTE* bits)
	{
		INT index = 0;
		ZeroMemory(&myAF, sizeof(myAF));
		myAF.AdaptationFieldLength = bits[index++];
		myAF.DiscontinuityIndicator = bits[index] >> 7;
		myAF.RandomAccessIndicator = (bits[index] >> 6) & 0x1;
		myAF.ElementaryStreamPriorityIndicator = (bits[index] >> 5) & 0x1;
		myAF.PCRFlag = (bits[index] >> 4) & 0x1;
		myAF.OPCRFlag = (bits[index] >> 3) & 0x1;
		myAF.SplicingPointFlag = (bits[index] >> 2) & 0x1;
		myAF.TransportPrivateDataFlag = (bits[index] >> 1) & 0x1;
		myAF.AdaptationFieldExtensionFlag = bits[index] & 0x1;
		index += 1;
		if (myAF.PCRFlag == 0x1)
		{
			myAF.ProgramClockReferenceBase = ((UINT64)bits[index] << 25) |
				(bits[index + 1] << 17) |
				(bits[index + 2] << 9) |
				(bits[index + 3] << 1) |
				(bits[index + 4] >> 7);
			myAF.ProgramClockReferenceExtension = ((bits[index + 4] & 0x1) << 8) | bits[index + 5];
			index += 6;
		}
		if (myAF.OPCRFlag)
		{
			myAF.OriginalProgramClockReferenceBase = ((UINT64)bits[index] << 25) |
				(bits[index + 1] << 17) |
				(bits[index + 2] << 9) |
				(bits[index + 3] << 1) |
				(bits[index + 4] >> 7);
			myAF.OriginalProgramClockReferenceExtension = ((bits[index + 4] & 0x1) << 8) | bits[index + 5];
			index += 6;
		}
		if (myAF.SplicingPointFlag)
		{
			myAF.SpliceType = bits[index++];
		}
		if (myAF.TransportPrivateDataFlag)
		{
			myAF.TransportPrivateDataLength = bits[index++];
			index += myAF.TransportPrivateDataLength;
		}
		if (myAF.AdaptationFieldExtensionFlag)
		{
			myAF.AdaptationFieldExtensionLength = bits[index++];
			myAF.ItwFlag = bits[index] >> 7;
			myAF.PiecewiseRateFlag = (bits[index] >> 6) & 0x1;
			myAF.SeamlessSpliceFlag = (bits[index] >> 5) & 0x1;
			index += 1;
			if (myAF.ItwFlag)
			{
				myAF.ItwValidFlag = bits[index] >> 7;
				myAF.ItwOffset = ((bits[index] & 0x7F) << 8) | bits[index + 1];
				index += 2;
			}
			if (myAF.PiecewiseRateFlag)
			{
				myAF.PiecewiseRate = ((bits[index] & 0x3F) << 16) | (bits[index + 1] << 8) | bits[index + 2];
				index += 3;
			}
			if (myAF.SeamlessSpliceFlag)
			{
				myAF.SpliceType = bits[index] >> 4;
				myAF.DTSNextAU = (((bits[index] >> 1) & 0x7) << 30) |
					(bits[index + 1] << 22) |
					((bits[index + 2] >> 1) << 15) |
					(bits[index + 3] << 8) |
					(bits[index + 4] >> 1);
			}
		}
		ASSERT(index == (myAF.AdaptationFieldLength + 1));
		return (myAF.AdaptationFieldLength + 1);
	}
	BOOL TSReader::ReadPAT(TS_PACKET_PAT* pPAT, TinyArray<TS_PACKET_PROGRAM>& programs, const BYTE* bits)
	{
		INT index = 0;
		pPAT->TableID = bits[index++];
		pPAT->SectionSyntaxIndicator = bits[index] >> 7;
		pPAT->Zero = bits[index] >> 6 & 0x1;
		pPAT->Reserved1 = bits[index] >> 4 & 0x3;
		pPAT->SectionLength = ((bits[index] & 0x0F) << 8) | bits[index + 1];
		index += 2;
		pPAT->TransportStreamID = ((bits[index] << 8) | bits[index + 1]);
		index += 2;
		pPAT->Reserved2 = bits[index] >> 6;
		pPAT->VersionNumber = (bits[index] >> 1) & 0x1F;
		pPAT->CurrentNextIndicator = (bits[index] << 7) >> 7;
		pPAT->SectionNumber = bits[++index];
		pPAT->LastSectionNumber = bits[++index];
		INT size = pPAT->SectionLength;
		pPAT->CRC32 = (bits[size - 4] & 0x000000FF) << 24 | (bits[size - 3] & 0x000000FF) << 16 | (bits[size - 2] & 0x000000FF) << 8 | (bits[size - 1] & 0x000000FF);
		if (pPAT->TableID != 0x0)
			return FALSE;
		if (pPAT->SectionSyntaxIndicator != 0x1)
			return FALSE;
		if (pPAT->Zero != 0x0)
			return FALSE;
		if (pPAT->SectionLength >= 1021)
			return FALSE;
		//PAT当前有效
		if (pPAT->CurrentNextIndicator != 0x1)
			return FALSE;
		//PAT没有改变
		if (m_versionNumber == pPAT->VersionNumber)
			return TRUE;
		for (;;)
		{
			if (index >= (size - 4))
				break;
			index += 1;
			TS_PACKET_PROGRAM program = { 0 };
			program.ProgramNumber = bits[index] << 8 | bits[index + 1];
			index += 2;
			program.Reserved = bits[index] >> 5;
			program.ProgramPID = (bits[index] & 0x1F) << 8 | bits[index + 1];
			index += 2;
			programs.Add(program);
		}
		return TRUE;
	}
	BOOL TSReader::ReadPTM(TS_PACKET_PMT* pPMT, TinyArray<TS_PACKET_STREAM*>& streams, const BYTE* bits)
	{
		INT index = 0;
		pPMT->TableID = bits[index++];
		pPMT->SectionSyntaxIndicator = bits[index] >> 7;
		pPMT->Zero = bits[index] >> 6 & 0x1;
		pPMT->Reserved1 = bits[index] >> 4 & 0x3;
		pPMT->SectionLength = ((bits[index] & 0x0F) << 8) | bits[index + 1];
		index += 2;
		pPMT->ProgramNumber = bits[index] << 8 | bits[index + 1];
		index += 2;
		pPMT->Reserved2 = bits[index] >> 6;
		pPMT->VersionNumber = bits[index] >> 1 & 0x1F;
		pPMT->CurrentNextIndicator = (bits[index] << 7) >> 7;
		pPMT->SectionNumber = bits[++index];
		pPMT->LastSectionNumber = bits[++index];
		index += 1;
		pPMT->Reserved3 = bits[index] >> 5;
		pPMT->PCR_PID = ((bits[index] << 8) | bits[index + 1]) & 0x1FFF;
		index += 2;
		pPMT->Reserved4 = bits[index] >> 4;
		pPMT->ProgramInfoLength = (bits[index] & 0x0F) << 8 | bits[index + 1];
		index += 2;
		index += pPMT->ProgramInfoLength;
		INT size = pPMT->SectionLength;
		pPMT->CRC32 = (bits[size - 4] & 0x000000FF) << 24 | (bits[size - 3] & 0x000000FF) << 16 | (bits[size - 2] & 0x000000FF) << 8 | (bits[size - 1] & 0x000000FF);
		if (pPMT->TableID != 0x2)
			return FALSE;
		if (pPMT->SectionSyntaxIndicator != 0x1)
			return FALSE;
		if (pPMT->Zero != 0x0)
			return FALSE;
		if (pPMT->SectionLength >= 1021)
			return FALSE;
		//PMT当前有效
		if (pPMT->CurrentNextIndicator != 0x1)
			return FALSE;
		if (pPMT->SectionNumber != 0)
			return FALSE;
		if (pPMT->LastSectionNumber != 0)
			return FALSE;
		for (;;)
		{
			if (index >= (size - 4))
				break;
			TS_PACKET_STREAM* stream = new TS_PACKET_STREAM();
			ZeroMemory(stream, sizeof(TS_PACKET_STREAM));
			stream->StreamType = bits[index++];
			stream->Reserved1 = bits[index] >> 5;
			stream->ElementaryPID = ((bits[index] << 8) | bits[index + 1]) & 0x1FFF;
			index += 2;
			stream->Reserved2 = bits[index] >> 4;
			stream->ESInfoLength = (bits[index] & 0x0F) << 8 | bits[index + 1];
			index += stream->ESInfoLength;
			index += 2;
			streams.Add(stream);
		}
		return TRUE;
	}
	BOOL TSReader::ReadPES(TS_PACKET_STREAM* stream, TS_PACKET_PES* pPES, const BYTE* bits, INT offset)
	{
		ASSERT(stream);
		INT index = 0;
		pPES->PacketStartCodePrefix = bits[index] << 16 | bits[index + 1] << 8 | bits[index + 2];
		if (pPES->PacketStartCodePrefix != 0x000001)
			return FALSE;
		index += 3;
		pPES->StreamID = bits[index++];
		pPES->PESPacketLength = bits[index] << 8 | bits[index + 1];
		index += 2;
		BOOL bAudio = ((pPES->StreamID & 0xe0) == 0xc0);
		BOOL bVideo = ((pPES->StreamID & 0xf0) == 0xe0);
		BOOL bPrivateStream11 = (pPES->StreamID == 0xbd);
		if (!bAudio && !bVideo && !bPrivateStream11)//抛弃
			return TRUE;
		pPES->PESScramblingControl = (bits[index] & 0x30) >> 4;
		pPES->PESPriority = (bits[index] & 0x08) >> 3;
		pPES->DataAlignmentIndicator = (bits[index] & 0x04) >> 2;
		pPES->Copyright = (bits[index] & 0x02) >> 1;
		pPES->OriginalOrCopy = (bits[index] & 0x01);
		index += 1;
		pPES->PTSDTSFlags = (bits[index] & 0xc0) >> 6;
		pPES->ESCRFlag = (bits[index] & 0x20) >> 5;
		pPES->ESRateFlag = (bits[index] & 0x10) >> 4;
		pPES->DSMTrickModeFlag = (bits[index] & 0x08) >> 3;
		pPES->AdditionalCopyInfoFlag = (bits[index] & 0x04) >> 2;
		pPES->PESCRCFlag = (bits[index] & 0x02) >> 1;
		pPES->PESExtensionFlag = (bits[index] & 0x01);
		index += 1;
		pPES->PESHeaderDataLength = bits[index++];
		if (pPES->PTSDTSFlags == 0x2)
		{
			if ((bits[index] >> 4 & 0xF) != 0x2)
				return FALSE;
			INT64 timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
				(bits[index + 1] << 22) |
				((bits[index + 2] >> 1) << 15) |
				(bits[index + 3] << 7) |
				(bits[index + 4] >> 1);
			pPES->PTS = (((timestamp >> 33) & 0x7) << 30) | (((timestamp >> 17) & 0x7FFF) << 15) | (((timestamp >> 1) & 0x7FFF) << 0);
			pPES->DTS = 0;
			index += 5;
		}
		if (pPES->PTSDTSFlags == 0x3)
		{
			if ((bits[index] >> 4 & 0xF) != 0x3)
				return FALSE;
			INT64 timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
				(bits[index + 1] << 22) |
				((bits[index + 2] >> 1) << 15) |
				(bits[index + 3] << 7) |
				(bits[index + 4] >> 1);
			pPES->PTS = (((timestamp >> 33) & 0x7) << 30) | (((timestamp >> 17) & 0x7FFF) << 15) | (((timestamp >> 1) & 0x7FFF) << 0);
			index += 5;
			if ((bits[index] >> 4 & 0xF) != 0x1)
				return FALSE;
			timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
				(bits[index + 1] << 22) |
				((bits[index + 2] >> 1) << 15) |
				(bits[index + 3] << 7) |
				(bits[index + 4] >> 1);
			pPES->DTS = (((timestamp >> 33) & 0x7) << 30) | (((timestamp >> 17) & 0x7FFF) << 15) | (((timestamp >> 1) & 0x7FFF) << 0);
			index += 5;
		}
		if (pPES->ESCRFlag)
		{
			pPES->ESCR = (((UINT64)(bits[index] >> 3) & 0x7) << 30) |
				(UINT64)((bits[index] & 0x3) << 28) |
				(UINT64)(bits[index + 1] << 20) |
				(UINT64)((bits[index + 2] >> 3) << 15) |
				(UINT64)((bits[index + 2] & 0x03) << 13) |
				(UINT64)(bits[index + 3] << 5) |
				(UINT64)(bits[index + 4] >> 3);
			index += 6;
		}
		if (pPES->ESRateFlag)
		{
			pPES->ESRate = ((bits[index] & 0x7F) << 15) | (bits[index + 1] << 7) | (bits[index + 2] >> 1);
			index += 3;
		}
		if (pPES->DSMTrickModeFlag)
		{
			pPES->TrickModeControl = bits[index] >> 5;
			if (pPES->TrickModeControl == 0x0 || pPES->TrickModeControl == 0x3)
			{
				pPES->FieldID = (bits[index] >> 3) & 0x3;
				pPES->IntraSliceRefresh = (bits[index] >> 2) & 0x1;
				pPES->FrequencyTruncation = bits[index] & 0x3;
			}
			else if (pPES->TrickModeControl == 0x1 || pPES->TrickModeControl == 0x4)
			{
				pPES->RepCntrl = bits[index] & 0x1F;
			}
			else if (pPES->TrickModeControl == 0x2)
			{
				pPES->FieldID = (bits[index] >> 3) & 0x3;
			}
			index += 1;
		}
		if (pPES->AdditionalCopyInfoFlag)
		{
			pPES->AdditionalCopyInfo = bits[index++] & 0x7F;
		}
		if (pPES->PESCRCFlag)
		{
			pPES->PreviousPESPacketCRC = (bits[index] << 8) | bits[index + 1];
			index += 2;
		}
		if (pPES->PESExtensionFlag)
		{
			pPES->PESPrivateDataFlag = bits[index] >> 7;
			pPES->PackFieldLength = (bits[index] >> 6) & 0x1;
			pPES->ProgramPacketSequenceCounterFlag = (bits[index] >> 5) & 0x1;
			pPES->PSTDBufferFlag = (bits[index] >> 4) & 0x1;
			pPES->PESExtensionFlag2 = bits[index] & 0x1;
			index += 1;
			if (pPES->PESPrivateDataFlag)
			{
				index += 16;//16位
			}
			if (pPES->PackHeaderFieldFlag)
			{
				pPES->PackFieldLength = bits[index++];
				index += pPES->PackFieldLength;
			}
			if (pPES->ProgramPacketSequenceCounterFlag)
			{
				pPES->ProgramPacketSequenceCounter = bits[index] & 0x7F;
				pPES->MPEG1MPEG2Identifier = (bits[index + 1] >> 6) & 0x1;
				pPES->OriginalStuffLength = bits[index + 1] & 0x3F;
				index += 2;
			}
			if (pPES->PSTDBufferFlag)
			{
				pPES->PSTDBufferScale = (bits[index] >> 5) & 0x1;
				pPES->PSTDBufferSize = ((bits[index] & 0x1F) << 8) | bits[index + 1];
				index += 2;
			}
			if (pPES->PESExtensionFlag2)
			{
				pPES->PESExtensionFieldLength = bits[index++] & 0x7F;
				pPES->StreamIDExtensionFlag = bits[index] >> 7;
				if (pPES->StreamIDExtensionFlag)
				{
					pPES->StreamIDExtension = bits[index++] & 0x7F;
					index += pPES->PESExtensionFieldLength;
				}
			}
		}
		ASSERT(index == pPES->PESHeaderDataLength + 9);
		TSParser* parser = stream->GetParser();
		if (parser != NULL)
		{
			if (pPES->PESPacketLength > 0)
			{
				parser->GetIO().SetCapacity(parser->GetIO().capacity() + pPES->PESPacketLength - 3 - pPES->PESHeaderDataLength);
			}
			else
			{
				parser->GetIO().SetCapacity(parser->GetIO().capacity() + 64 * 1024);
			}
			INT size = TS_PACKET_SIZE - (offset + index);
			memcpy_s((void*)parser->GetIO().data(), size, (BYTE*)(bits + index), size);
		}
		return TRUE;
	}
	BOOL TSReader::ParseAAC(const BYTE* bits, LONG size)
	{
		return TRUE;
	}
	BOOL TSReader::ParseH264(const BYTE* bits, LONG size)
	{
		return TRUE;
	}
	BOOL TSReader::ReadPacket(TS_PACKEG_HEADER& header, TS_BLOCK& block)
	{
		//TS传输包固定188个字节
		ZeroMemory(&header, sizeof(header));
		ULONG ls = 0;
		HRESULT hRes = S_OK;
		hRes = m_stream->Read(m_bits, TS_PACKET_SIZE, &ls);
		if (hRes != S_OK || ls != TS_PACKET_SIZE)
			return FALSE;
		INT index = 0;
		header.Syncbyte = m_bits[0];
		if (header.Syncbyte != 0x47)
			return FALSE;
		header.TransportErrorIndicator = m_bits[1] >> 7;
		if (header.TransportErrorIndicator != 0)//校验错误
			return FALSE;
		header.PayloadUnitStartIndicator = m_bits[1] >> 6 & 0x01;
		header.TransportPriority = m_bits[1] >> 5 & 0x01;
		header.PID = (m_bits[1] & 0x1F) << 8 | m_bits[2];
		header.TransportScramblingControl = m_bits[3] >> 6;
		header.AdaptationFieldControl = (m_bits[3] >> 4) & 0x03;
		header.ContinuityCounter = m_bits[3] & 0x0F;
		index += 4;
		INT continuityCounter = (m_continuityCounter + 1) % 16;
		if (m_continuityCounter >= 0 && header.ContinuityCounter != continuityCounter)
			return FALSE;
		if (header.AdaptationFieldControl == 0x2 || header.AdaptationFieldControl == 0x3)
		{
			TS_PACKET_ADAPTATION_FIELD myAF;
			index += ReadAF(myAF, m_bits + index);
		}

		if (header.AdaptationFieldControl == 0x1 || header.AdaptationFieldControl == 0x3)
		{
			if (header.PID == 0x0000)//PAT
			{
				if (header.PayloadUnitStartIndicator)//表示带有pointer_field
				{
					INT pointer = m_bits[index++] & 0x0F;//有效载荷的位置
					index += pointer;
				}
				TinyScopedArray<TS_PACKET_SECTION> tcps(new TS_PACKET_PAT());
				if (!tcps)
					return FALSE;
				ZeroMemory(tcps, sizeof(TS_PACKET_PAT));
				m_programs.RemoveAll();
				if (!ReadPAT(static_cast<TS_PACKET_PAT*>(tcps.Ptr()), m_programs, m_bits + index))
					return FALSE;
				m_versionNumber = static_cast<TS_PACKET_PAT*>(tcps.Ptr())->VersionNumber;
			}
			if (header.PID >= 0x0010)
			{
				//PES
				for (INT i = 0;i < m_streams.GetSize();i++)
				{
					TS_PACKET_STREAM* stream = m_streams[i];
					if (stream->ElementaryPID == header.PID)
					{
						if (header.PayloadUnitStartIndicator)
						{
							TS_PACKET_PES myPES;
							ZeroMemory(&myPES, sizeof(myPES));
							if (!ReadPES(stream, &myPES, m_bits + index, index))
								return FALSE;
						}
						else
						{
							TSParser* parser = stream->GetParser();
							if (parser != NULL)
							{
								if (parser->GetIO().RemainingCapacity() <= TS_PACKET_SIZE)
								{
									parser->GetIO().SetCapacity(parser->GetIO().capacity() + 64 * 1024);
								}
								INT size = TS_PACKET_SIZE - index;
								memcpy_s((void*)parser->GetIO().data(), size, (BYTE*)(m_bits + index), size);
							}
						}
						break;
					}
				}
				//PMT
				for (INT i = 0;i < m_programs.GetSize();i++)
				{
					TS_PACKET_PROGRAM& program = m_programs[i];
					if (program.ProgramPID == header.PID)
					{
						if (header.PayloadUnitStartIndicator)//表示带有pointer_field
						{
							INT pointer = m_bits[index++] & 0x0F;//有效载荷的位置
							index += pointer;
						}
						TinyScopedArray<TS_PACKET_SECTION> tcps(new TS_PACKET_PMT());
						if (!tcps)
							return FALSE;
						m_streams.RemoveAll();
						if (!ReadPTM(static_cast<TS_PACKET_PMT*>(tcps.Ptr()), m_streams, m_bits + index))
							return FALSE;
						break;
					}
				}
			}
		}
		return TRUE;
	}
	BOOL TSReader::Close()
	{
		for (INT i = 0;i < m_streams.GetSize();i++)
			SAFE_DELETE(m_streams[i]);
		m_streams.RemoveAll();
		m_programs.RemoveAll();
		m_stream.Release();
		m_versionNumber = -1;
		m_continuityCounter = -1;
		ZeroMemory(m_bits, TS_PACKET_SIZE);
		return TRUE;
	}
}

