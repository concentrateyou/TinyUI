#include "stdafx.h"
#include "TSReader.h"

namespace Decode
{
	INT64 ConvertToTimestamp(INT64 timestamp)
	{
		return (((timestamp >> 33) & 0x7) << 30) |
			(((timestamp >> 17) & 0x7fff) << 15) |
			(((timestamp >> 1) & 0x7fff) << 0);
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
	TSReader::TSReader()
	{
		ZeroMemory(m_bits, 188);
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
			TS_PACKET_SECTION* ps = NULL;
			INT iRes = ReadPacket(header, ps);
			SAFE_DELETE(ps);
			if (iRes < 0)
				return FALSE;
		}
	}
	INT TSReader::ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, BYTE* data)
	{
		INT index = 0;
		ZeroMemory(&myAF, sizeof(myAF));
		myAF.AdaptationFieldLength = data[index++];
		myAF.DiscontinuityIndicator = data[index] >> 7;
		myAF.RandomAccessIndicator = (data[index] >> 6) & 0x1;
		myAF.ElementaryStreamPriorityIndicator = (data[index] >> 5) & 0x1;
		myAF.PCRFlag = (data[index] >> 4) & 0x1;
		myAF.OPCRFlag = (data[index] >> 3) & 0x1;
		myAF.SplicingPointFlag = (data[index] >> 2) & 0x1;
		myAF.TransportPrivateDataFlag = (data[index] >> 1) & 0x1;
		myAF.AdaptationFieldExtensionFlag = data[index] & 0x1;
		index += 1;
		if (myAF.PCRFlag == 0x1)
		{
			myAF.ProgramClockReferenceBase = ((UINT64)data[index] << 25) |
				(data[index + 1] << 17) |
				(data[index + 2] << 9) |
				(data[index + 3] << 1) |
				(data[index + 4] >> 7);
			myAF.ProgramClockReferenceExtension = ((data[index + 4] & 0x1) << 8) | data[index + 5];
			index += 6;
		}
		if (myAF.OPCRFlag)
		{
			myAF.OriginalProgramClockReferenceBase = ((UINT64)data[index] << 25) |
				(data[index + 1] << 17) |
				(data[index + 2] << 9) |
				(data[index + 3] << 1) |
				(data[index + 4] >> 7);
			myAF.OriginalProgramClockReferenceExtension = ((data[index + 4] & 0x1) << 8) | data[index + 5];
			index += 6;
		}
		if (myAF.SplicingPointFlag)
		{
			myAF.SpliceType = data[index++];
		}
		if (myAF.TransportPrivateDataFlag)
		{
			myAF.TransportPrivateDataLength = data[index++];
			//DOTO
			index += myAF.TransportPrivateDataLength;

		}
		if (myAF.AdaptationFieldExtensionFlag)
		{
			myAF.AdaptationFieldExtensionLength = data[index++];
			myAF.ItwFlag = data[index] >> 7;
			myAF.PiecewiseRateFlag = (data[index] >> 6) & 0x1;
			myAF.SeamlessSpliceFlag = (data[index] >> 5) & 0x1;
			index += 1;
			if (myAF.ItwFlag)
			{
				myAF.ItwValidFlag = data[index] >> 7;
				myAF.ItwOffset = ((data[index] & 0x7F) << 8) | data[index + 1];
				index += 2;
			}
			if (myAF.PiecewiseRateFlag)
			{
				myAF.PiecewiseRate = ((data[index] & 0x3F) << 16) | (data[index + 1] << 8) | data[index + 2];
				index += 3;
			}
			if (myAF.SeamlessSpliceFlag)
			{
				myAF.SpliceType = data[index] >> 4;
				myAF.DTSNextAU = (((data[index] >> 1) & 0x7) << 30) |
					(data[index + 1] << 22) |
					((data[index + 2] >> 1) << 15) |
					(data[index + 3] << 8) |
					(data[index + 4] >> 1);
			}
		}
		return (myAF.AdaptationFieldLength + 1);
	}
	INT TSReader::ReadPAT(TS_PACKET_PAT* pPAT, TinyArray<TS_PACKET_PROGRAM>& programs, const BYTE* bits)
	{
		programs.RemoveAll();
		INT index = 0;
		pPAT->TransportStreamID = ((bits[index] << 8) | bits[index + 1]);
		index += 2;
		pPAT->Reserved2 = bits[index] >> 6;
		pPAT->VersionNumber = (bits[index] >> 1) & 0x1F;
		pPAT->CurrentNextIndicator = (bits[index] << 7) >> 7;
		pPAT->SectionNumber = bits[++index];
		pPAT->LastSectionNumber = bits[++index];
		INT size = pPAT->SectionLength;
		pPAT->CRC32 = (bits[size - 4] & 0x000000FF) << 24 | (bits[size - 3] & 0x000000FF) << 16 | (bits[size - 2] & 0x000000FF) << 8 | (bits[size - 1] & 0x000000FF);
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
		return (size + 1);
	}
	INT	TSReader::ReadPTM(TS_PACKET_PMT* pPMT, TinyArray<TS_PACKET_STREAM>& streams, const BYTE* bits)
	{
		m_streams.RemoveAll();
		INT index = 0;
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

		for (;;)
		{
			if (index >= (size - 4))
				break;
			TS_PACKET_STREAM stream;
			ZeroMemory(&stream, sizeof(stream));
			stream.StreamType = bits[index++];
			stream.Reserved1 = bits[index] >> 5;
			stream.ElementaryPID = ((bits[index] << 8) | bits[index + 1]) & 0x1FFF;
			index += 2;
			stream.Reserved2 = bits[index] >> 4;
			stream.ESInfoLength = (bits[index] & 0x0F) << 8 | bits[index + 1];
			index += stream.ESInfoLength;
			index += 2;
			streams.Add(stream);
		}
		return (size + 1);
	}
	BOOL TSReader::ReadPES(const TS_PACKET_STREAM& stream, TS_PACKET_PES* pPES, const BYTE* bits)
	{
		ZeroMemory(pPES, sizeof(TS_PACKET_PES));
		INT index = 0;
		pPES->StartCodePrefix = bits[index] << 16 | bits[index + 1] << 8 | bits[index + 2];
		if (pPES->StartCodePrefix != 0x000001)
			return FALSE;
		index += 3;
		pPES->StreamID = bits[index++];
		pPES->PESPacketLength = bits[index] << 8 | bits[index + 1];
		index += 2;
		if (pPES->StreamID != PROGRAM_STREAM_MAP &&
			pPES->StreamID != PADDING_STREAM           &&
			pPES->StreamID != PRIVATE_STREAM_2         &&
			pPES->StreamID != ECM_STREAM               &&
			pPES->StreamID != EMM_STREAM               &&
			pPES->StreamID != PROGRAM_STREAM_DIRECTORY &&
			pPES->StreamID != DSMCC_STREAM            &&
			pPES->StreamID != H222_TYPE_E)
		{
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
				INT64 timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
					(bits[index + 1] << 22) |
					((bits[index + 2] >> 1) << 15) |
					(bits[index + 3] << 7) |
					(bits[index + 4] >> 1);
				if ((timestamp >> 36 & 0xF) != 0x2)
					return FALSE;
				if (((timestamp & 0x1) == 0) ||
					((timestamp & 0x10000) == 0) ||
					((timestamp & 0x100000000) == 0))
					return FALSE;
				pPES->PTS = (((timestamp >> 33) & 0x7) << 30) | (((timestamp >> 17) & 0x7FFF) << 15) | (((timestamp >> 1) & 0x7FFF) << 0);;
				pPES->DTS = 0;
				index += 5;
			}
			if (pPES->PTSDTSFlags == 0x3)
			{
				INT64 timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
					(bits[index + 1] << 22) |
					((bits[index + 2] >> 1) << 15) |
					(bits[index + 3] << 7) |
					(bits[index + 4] >> 1);
				if ((timestamp >> 36 & 0xF) != 0x2)
					return FALSE;
				if (((timestamp & 0x1) == 0) ||
					((timestamp & 0x10000) == 0) ||
					((timestamp & 0x100000000) == 0))
					return FALSE;
				pPES->PTS = (((timestamp >> 33) & 0x7) << 30) | (((timestamp >> 17) & 0x7FFF) << 15) | (((timestamp >> 1) & 0x7FFF) << 0);
				index += 5;
				timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
					(bits[index + 1] << 22) |
					((bits[index + 2] >> 1) << 15) |
					(bits[index + 3] << 7) |
					(bits[index + 4] >> 1);
				if ((timestamp >> 36 & 0xF) != 0x1)
					return FALSE;
				if (((timestamp & 0x1) == 0) ||
					((timestamp & 0x10000) == 0) ||
					((timestamp & 0x100000000) == 0))
					return FALSE;
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
		}
		index += pPES->PESPacketLength;
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

	BOOL TSReader::ReadPacket(TS_PACKEG_HEADER& header, TS_PACKET_SECTION*& ps)
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
		if (header.AdaptationFieldControl == 0x2 || header.AdaptationFieldControl == 0x3)
		{
			TS_PACKET_ADAPTATION_FIELD myAF;
			index += ReadAF(myAF, m_bits + index);
		}
		if (header.AdaptationFieldControl == 0x1 || header.AdaptationFieldControl == 0x3)
		{
			if (header.PayloadUnitStartIndicator == 0x1)//表示带有pointer_field
			{
				INT pointer = m_bits[index] & 0x0F;//有效载荷的位置
				index += 1;
				index += pointer;
			}
			if (header.PID == 0x0000)//PAT
			{
				TS_PACKET_PAT* pPAT = new TS_PACKET_PAT();
				pPAT->TableID = m_bits[index++];
				pPAT->SectionSyntaxIndicator = m_bits[index] >> 7;
				pPAT->Zero = m_bits[index] >> 6 & 0x1;
				pPAT->Reserved1 = m_bits[index] >> 4 & 0x3;
				pPAT->SectionLength = ((m_bits[index] & 0x0F) << 8) | m_bits[index + 1];
				ASSERT(pPAT->TableID == 0x0);
				ASSERT(pPAT->SectionSyntaxIndicator);
				ASSERT(!pPAT->Zero);
				ps = static_cast<TS_PACKET_SECTION*>(pPAT);
				index += 2;
				index += ReadPAT(pPAT, m_programs, m_bits + index);
			}
			if (header.PID >= 0x0010)
			{
				for (INT i = 0;i < m_streams.GetSize();i++)
				{
					TS_PACKET_STREAM& stream = m_streams[i];
					if (stream.ElementaryPID == header.PID)
					{
						TS_PACKET_PES myPES;
						ZeroMemory(&myPES, sizeof(myPES));
						if (!ReadPES(stream, &myPES, m_bits + index))
							return FALSE;
					}
				}
				for (INT i = 0;i < m_programs.GetSize();i++)
				{
					TS_PACKET_PROGRAM& program = m_programs[i];
					if (program.ProgramPID == header.PID)
					{
						TS_PACKET_PMT* pPMT = new TS_PACKET_PMT();
						pPMT->TableID = m_bits[index++];
						pPMT->SectionSyntaxIndicator = m_bits[index] >> 7;
						pPMT->Zero = m_bits[index] >> 6 & 0x1;
						pPMT->Reserved1 = m_bits[index] >> 4 & 0x3;
						pPMT->SectionLength = ((m_bits[index] & 0x0F) << 8) | m_bits[index + 1];
						ASSERT(pPMT->TableID == 0x2);
						ASSERT(pPMT->SectionSyntaxIndicator);
						ASSERT(!pPMT->Zero);
						ps = static_cast<TS_PACKET_SECTION*>(pPMT);
						index += 2;
						index += ReadPTM(pPMT, m_streams, m_bits + index);
					}
				}
			}
		}
		return index;
	}
	BOOL TSReader::Close()
	{
		m_stream.Release();
		return TRUE;
	}
}

