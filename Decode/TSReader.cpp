#include "stdafx.h"
#include "TSReader.h"

namespace Decode
{
	TSReader::TSReader()
		:m_offset(0)
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
	INT TSReader::ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, BYTE* bits)
	{
		INT index = 0;
		ZeroMemory(&myAF, sizeof(myAF));
		myAF.AdaptationFieldExtensionLength = bits[index++];
		if (myAF.AdaptationFieldExtensionLength > 0)
		{
			myAF.DiscontinuityIndicator = bits[index] >> 7;
			myAF.RandomAccessIndicator = (bits[index] >> 6) & 0x1;
			myAF.ElementaryStreamPriorityIndicator = (bits[index] >> 5) & 0x1;
			myAF.PCRFlag = (bits[index] >> 4) & 0x1;
			myAF.OPCRFlag = (bits[index] >> 3) & 0x1;
			myAF.SplicingPointFlag = (bits[index] >> 2) & 0x1;
			myAF.TransportPrivateDataFlag = (bits[index] >> 1) & 0x1;
			myAF.AdaptationFieldExtensionFlag = bits[index] & 0x1;
			index++;
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
				//DOTO
				index += myAF.TransportPrivateDataLength;

			}
			if (myAF.AdaptationFieldExtensionFlag)
			{
				myAF.AdaptationFieldExtensionLength = bits[index++];
				myAF.ItwFlag = bits[index] >> 7;
				myAF.PiecewiseRateFlag = (bits[index] >> 6) & 0x1;
				myAF.SeamlessSpliceFlag = (bits[index] >> 5) & 0x1;
				index++;
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
		}
		ASSERT(index <= myAF.AdaptationFieldExtensionLength);
		return (myAF.AdaptationFieldExtensionLength + 1);
	}
	INT TSReader::ReadPAT(TS_PACKEG_PAT& myPAT, TinyArray<TS_PACKET_PROGRAM>& programs, BYTE* bits)
	{
		INT index = 0;
		ZeroMemory(&myPAT, sizeof(myPAT));
		myPAT.TableID = bits[index + 1];
		index += 1;
		myPAT.SectionSyntaxIndicator = bits[index] >> 7;
		myPAT.Zero = bits[index] >> 6 & 0x1;
		myPAT.Reserved1 = bits[index] >> 4 & 0x3;
		myPAT.SectionLength = (bits[index] & 0x0F) << 8 | bits[index + 1];
		index += 1;
		//3字节
		myPAT.TransportStreamID = ((bits[index + 1] << 8) | bits[index + 2]);
		index += 2;
		myPAT.Reserved2 = bits[index + 1] >> 6;
		index += 1;
		myPAT.VersionNumber = (bits[index] >> 1) & 0x1F;
		myPAT.CurrentNextIndicator = (bits[index] << 7) >> 7;
		myPAT.SectionNumber = bits[index + 1];
		index += 1;
		myPAT.LastSectionNumber = bits[index + 1];
		index += 1;
		INT size = 3 + myPAT.SectionLength;//总长度
		myPAT.CRC32 = = (bits[size - 4] & 0x000000FF) << 24 |
			(bits[size - 3] & 0x000000FF) << 16 |
			(bits[size - 2] & 0x000000FF) << 8 |
			(bits[size - 1] & 0x000000FF);
		for (;;)
		{
			TS_PACKET_PROGRAM program = { 0 };
			program.ProgramNumber = bits[index + 1] << 8 | bits[index + 2];
			index += 2;
			program.Reserved = bits[index++] >> 5;
			program.ProgramPID = (bits[index] & 0x1F) << 8 | bits[++index];
			programs.Add(program);
		}
	}
	BOOL TSReader::ReadPacket()
	{
		//TS传输包固定188个字节
		ULONG ls = 0;
		HRESULT hRes = m_stream->Read(m_bits, 188, &ls);
		if (hRes != S_OK || ls != 188)
			return FALSE;
		INT offset = 0;
		TS_PACKEG_HEADER header = { 0 };
		header.Syncbyte = m_bits[0];
		if (header.Syncbyte != 0x47)
			return FALSE;
		header.TransportErrorIndicator = m_bits[1] & 0x80;
		if (header.TransportErrorIndicator != 0)//校验错误
			return FALSE;
		header.PayloadUnitStartIndicator = m_bits[1] & 0x40;
		header.TransportPriority = m_bits[1] & 0x20;
		header.PID = (m_bits[1] & 0x1F) << 8 | m_bits[2];
		header.TransportScramblingControl = m_bits[3] & 0xC0;
		header.AdaptationFieldControl = m_bits[3] & 0x30;
		header.ContinuityCounter = m_bits[3] & 0x0F;
		offset += 4;
		if (header.AdaptationFieldControl == 0x2 || header.AdaptationFieldControl == 0x3)
		{
			TS_PACKET_ADAPTATION_FIELD myAF = { 0 };
			INT iRes = ReadAF(myAF, m_bits + offset);
			offset += iRes;
		}
		if (header.PID == 0x0000)//PAT
		{
			if (header.PayloadUnitStartIndicator == 0x01)//存在PSI
			{
				TS_PACKEG_PAT myPAT;
				INT iRes = ReadPAT(myAF, m_bits + offset);
				offset += iRes;
			}
		}
		if (header.PID == 0x0001)//CAT
		{

		}
		if (header.PID == 0x0002)//TSDT
		{

		}
		if (header.PID == 0x0012)//EIT,ST
		{

		}
		if (header.PID == 0x0013)//RST,ST
		{

		}
		if (header.PID == 0x0014)//TDT,TOT,ST
		{

		}
		if (header.PID == 0x1FFF)
		{

		}
		return TRUE;
	}
	BOOL TSReader::Close()
	{
		m_stream.Release();
		return TRUE;
	}
}

