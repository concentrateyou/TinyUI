#include "stdafx.h"
#include "TSReader.h"

namespace Decode
{
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
	INT	TSReader::ReadSection(TS_PACKET_SECTION& section, BYTE* data)
	{
		INT index = 0;
		section.TableID = data[index++];
		section.SectionSyntaxIndicator = data[index] >> 7;
		section.Zero = data[index] >> 6;
		section.Reserved1 = data[index] >> 4;
		section.SectionLength = (data[index] & 0x0F) << 8 | data[index + 1];
		index += 2;
		return index;
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
	INT TSReader::ReadPAT(TS_PACKET_PAT* pPAT, TinyArray<TS_PACKET_PROGRAM>& programs, BYTE* bits)
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
	INT	TSReader::ReadPTM(TS_PACKET_PMT* pPMT, TinyArray<TS_PACKET_STREAM>& streams, BYTE* bits)
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
		if (pPMT->ProgramInfoLength > 0)
		{
			ParseDescriptor(bits + index, pPMT->ProgramInfoLength);
		}
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
	INT	TSReader::ReadPES(BYTE* bits)
	{

	}
	INT TSReader::ReadPacket(TS_PACKEG_HEADER& header, TS_PACKET_SECTION*& ps)
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
	string	TSReader::ParseDescriptor(BYTE* bits, INT size)
	{
		string value;

		return value;
	}
	BOOL TSReader::Close()
	{
		m_stream.Release();
		return TRUE;
	}
}

