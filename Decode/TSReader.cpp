#include "stdafx.h"
#include "TSReader.h"

namespace Decode
{
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
	INT TSReader::ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, BYTE* data)
	{
		INT index = 0;
		ZeroMemory(&myAF, sizeof(myAF));
		myAF.AdaptationFieldLength = data[index++];
		if (myAF.AdaptationFieldLength > 0)
		{
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
		}
		ASSERT(index <= myAF.AdaptationFieldExtensionLength);
		return (myAF.AdaptationFieldExtensionLength + 1);
	}
	INT TSReader::ReadPAT(TS_PACKEG_PAT& myPAT, TinyArray<TS_PACKET_PROGRAM>& programs, BYTE* data)
	{
		INT index = 0;
		ZeroMemory(&myPAT, sizeof(myPAT));
		myPAT.TableID = data[index++];
		myPAT.SectionSyntaxIndicator = data[index] >> 7;
		myPAT.Zero = data[index] >> 6 & 0x1;
		myPAT.Reserved1 = data[index] >> 4 & 0x3;
		myPAT.SectionLength = ((data[index] & 0x0F) << 8) | data[index + 1];
		index += 2;
		//3字节
		myPAT.TransportStreamID = ((data[index] << 8) | data[index + 1]);
		index += 2;
		myPAT.Reserved2 = data[index] >> 6;
		myPAT.VersionNumber = (data[index] >> 1) & 0x1F;
		myPAT.CurrentNextIndicator = (data[index] << 7) >> 7;
		myPAT.SectionNumber = data[++index];
		myPAT.LastSectionNumber = data[++index];
		INT size = 3 + myPAT.SectionLength;//总长度
		myPAT.CRC32 = (data[size - 4] & 0x000000FF) << 24 | (data[size - 3] & 0x000000FF) << 16 | (data[size - 2] & 0x000000FF) << 8 | (data[size - 1] & 0x000000FF);
		for (;;)
		{
			if (index >= (size - 4))
				break;
			index += 1;
			TS_PACKET_PROGRAM program = { 0 };
			program.ProgramNumber = data[index] << 8 | data[index + 1];
			index += 2;
			program.Reserved = data[index] >> 5;
			program.ProgramPID = (data[index] & 0x1F) << 8 | data[index + 1];
			index += 2;
			programs.Add(program);
		}
		return (size + 1);
	}
	BOOL TSReader::ReadPacket()
	{
		//TS传输包固定188个字节
		ULONG ls = 0;
		HRESULT hRes = S_OK;
		for (;;)
		{
			hRes = m_stream->Read(m_bits, 188, &ls);
			if (hRes != S_OK || ls != 188)
				return FALSE;
			INT offset = 0;
			TS_PACKEG_HEADER header = { 0 };
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
			offset += 5;
			if (header.AdaptationFieldControl == 0x2 || header.AdaptationFieldControl == 0x3)
			{
				TS_PACKET_ADAPTATION_FIELD myAF = { 0 };
				INT iRes = ReadAF(myAF, m_bits + offset - 1);
				offset += iRes;
			}
			if (header.PID == 0x0000)//PAT
			{
				if (header.PayloadUnitStartIndicator == 0x01)//存在PSI
				{
					m_programs.RemoveAll();
					TS_PACKEG_PAT myPAT;
					INT iRes = ReadPAT(myPAT, m_programs, m_bits + offset);
					offset += iRes;
				}
			}
			if (header.PID > 0x0000)
			{
				for (INT i = 0;i < m_programs.GetSize();i++)
				{
				}
			}
		}

		return TRUE;
	}
	BOOL TSReader::Close()
	{
		m_stream.Release();
		return TRUE;
	}
}

