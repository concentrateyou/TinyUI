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
	TSParser::TSParser()
		:m_capacity(0)
	{

	}
	TSParser::~TSParser()
	{

	}
	void TSParser::SetCapacity(INT capacity)
	{
		m_io.SetCapacity(capacity);
		m_capacity = capacity;
	}
	void TSParser::Add(BYTE* bits, INT size)
	{
		m_io.Add(bits, size);
	}
	void TSParser::Reset()
	{
		m_io.Clear();
		m_capacity = 0;
	}
	BYTE* TSParser::data()
	{
		return m_io.GetPointer();
	}
	INT TSParser::size() const
	{
		return m_io.GetSize();
	}
	void TSParser::SetConfig(vector<BYTE>& config)
	{
		m_config = std::move(config);
	}
	//////////////////////////////////////////////////////////////////////////
	TSH264Parser::TSH264Parser()
	{
	}
	TSH264Parser::~TSH264Parser()
	{

	}
	BYTE TSH264Parser::GetStreamType() const
	{
		return TS_STREAM_TYPE_VIDEO_H264;
	}
	BOOL TSH264Parser::Parse(TS_BLOCK& block)
	{
		if (m_parser.Parse(data(), size()))
		{

		}
		block.video.size = size();
		block.video.data = new BYTE[block.video.size];
		memcpy_s(block.video.data, block.video.size, data(), block.video.size);
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	static INT AAC_Sample_Rates[16] =
	{
		96000, 88200, 64000, 48000, 44100, 32000,
		24000, 22050, 16000, 12000, 11025, 8000, 7350,
		0, 0, 0
	};
	TSAACParser::TSAACParser()
	{
		m_asc.resize(2);
	}
	TSAACParser::~TSAACParser()
	{

	}
	BYTE TSAACParser::GetStreamType() const
	{
		return TS_STREAM_TYPE_AUDIO_AAC;
	}
	BOOL TSAACParser::Parse(TS_BLOCK& block)
	{
		block.audio.size = size();
		block.audio.data = new BYTE[block.audio.size];
		memcpy_s(block.audio.data, block.audio.size, data(), block.audio.size);
		return ParseADTS(block.audio.data, block.audio.size);
	}
	BOOL TSAACParser::ParseADTS(BYTE* bits, INT size)
	{
		if (size < ADTS_HEADER_MIN_SIZE)
			return FALSE;
		TinyBitReader reader;
		reader.Initialize(bits, size);
		if (bits[0] == 0xFF && (bits[1] & 0xF6) == 0xF0)
		{
			BYTE profile = 0;
			BYTE channel = 0;
			BYTE samplesPerSec = 0;
			reader.SkipBits(16);
			reader.ReadBits(2, &profile);
			reader.ReadBits(4, &samplesPerSec);
			reader.SkipBits(1);
			reader.ReadBits(3, &channel);
			reader.SkipBits(4);
			m_asc[0] &= profile >> 3;
			return TRUE;
		}
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
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
			{
				m_parser.Reset(new TSAACParser());
			}
			if (StreamType == TS_STREAM_TYPE_VIDEO_H264)
			{
				m_parser.Reset(new TSH264Parser());
			}
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
			ZeroMemory(&block, sizeof(block));
			TS_PACKEG_HEADER header;
			if (!ReadPacket(header, block))
				return FALSE;
			if (block.streamType > 0)
			{
				break;
			}
		}
		return TRUE;
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
		if (myAF.PCRFlag)
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
				index += 5;
			}
		}
		return (myAF.AdaptationFieldLength + 1);
	}
	BOOL TSReader::ReadPAT(TS_PACKET_PAT& myPAT, TinyArray<TS_PACKET_PROGRAM>& programs, const BYTE* bits)
	{
		INT index = 0;
		myPAT.TableID = bits[index++];
		myPAT.SectionSyntaxIndicator = bits[index] >> 7;
		myPAT.Zero = bits[index] >> 6 & 0x1;
		myPAT.Reserved1 = bits[index] >> 4 & 0x3;
		myPAT.SectionLength = ((bits[index] & 0x0F) << 8) | bits[index + 1];
		index += 2;
		myPAT.TransportStreamID = ((bits[index] << 8) | bits[index + 1]);
		index += 2;
		myPAT.Reserved2 = bits[index] >> 6;
		myPAT.VersionNumber = (bits[index] >> 1) & 0x1F;
		myPAT.CurrentNextIndicator = (bits[index] << 7) >> 7;
		myPAT.SectionNumber = bits[++index];
		myPAT.LastSectionNumber = bits[++index];
		INT size = myPAT.SectionLength;
		myPAT.CRC32 = (bits[size - 4] & 0x000000FF) << 24 | (bits[size - 3] & 0x000000FF) << 16 | (bits[size - 2] & 0x000000FF) << 8 | (bits[size - 1] & 0x000000FF);
		if (myPAT.TableID != 0x0)
			return FALSE;
		if (myPAT.SectionSyntaxIndicator != 0x1)
			return FALSE;
		if (myPAT.Zero != 0x0)
			return FALSE;
		if (myPAT.SectionLength >= 1021)
			return FALSE;
		//PAT当前有效
		if (myPAT.CurrentNextIndicator != 0x1)
			return FALSE;
		//PAT没有改变
		if (m_versionNumber == myPAT.VersionNumber)
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
	BOOL TSReader::ReadPTM(TS_PACKET_PMT& myPMT, TinyArray<TS_PACKET_STREAM*>& streams, const BYTE* bits)
	{
		INT index = 0;
		myPMT.TableID = bits[index++];
		myPMT.SectionSyntaxIndicator = bits[index] >> 7;
		myPMT.Zero = bits[index] >> 6 & 0x1;
		myPMT.Reserved1 = bits[index] >> 4 & 0x3;
		myPMT.SectionLength = ((bits[index] & 0x0F) << 8) | bits[index + 1];
		index += 2;
		myPMT.ProgramNumber = bits[index] << 8 | bits[index + 1];
		index += 2;
		myPMT.Reserved2 = bits[index] >> 6;
		myPMT.VersionNumber = bits[index] >> 1 & 0x1F;
		myPMT.CurrentNextIndicator = (bits[index] << 7) >> 7;
		myPMT.SectionNumber = bits[++index];
		myPMT.LastSectionNumber = bits[++index];
		index += 1;
		myPMT.Reserved3 = bits[index] >> 5;
		myPMT.PCR_PID = ((bits[index] << 8) | bits[index + 1]) & 0x1FFF;
		index += 2;
		myPMT.Reserved4 = bits[index] >> 4;
		myPMT.ProgramInfoLength = (bits[index] & 0x0F) << 8 | bits[index + 1];
		index += 2;
		index += myPMT.ProgramInfoLength;
		INT size = myPMT.SectionLength;
		myPMT.CRC32 = (bits[size - 4] & 0x000000FF) << 24 | (bits[size - 3] & 0x000000FF) << 16 | (bits[size - 2] & 0x000000FF) << 8 | (bits[size - 1] & 0x000000FF);
		if (myPMT.TableID != 0x2)
			return FALSE;
		if (myPMT.SectionSyntaxIndicator != 0x1)
			return FALSE;
		if (myPMT.Zero != 0x0)
			return FALSE;
		if (myPMT.SectionLength >= 1021)
			return FALSE;
		//PMT当前有效
		if (myPMT.CurrentNextIndicator != 0x1)
			return FALSE;
		if (myPMT.SectionNumber != 0)
			return FALSE;
		if (myPMT.LastSectionNumber != 0)
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
	BOOL TSReader::ReadPES(TS_PACKET_STREAM* stream, TS_PACKET_PES& myPES, TS_BLOCK& block, const BYTE* bits, INT offset)
	{
		ASSERT(stream);
		INT index = 0;
		myPES.PacketStartCodePrefix = bits[index] << 16 | bits[index + 1] << 8 | bits[index + 2];
		if (myPES.PacketStartCodePrefix != 0x000001)
			return FALSE;
		index += 3;
		myPES.StreamID = bits[index++];
		myPES.PESPacketLength = bits[index] << 8 | bits[index + 1];
		index += 2;
		BOOL bAudio = ((myPES.StreamID & 0xe0) == 0xc0);
		BOOL bVideo = ((myPES.StreamID & 0xf0) == 0xe0);
		BOOL bPrivateStream11 = (myPES.StreamID == 0xbd);
		if (!bAudio && !bVideo && !bPrivateStream11)//抛弃
			return TRUE;
		myPES.PESScramblingControl = (bits[index] & 0x30) >> 4;
		myPES.PESPriority = (bits[index] & 0x08) >> 3;
		myPES.DataAlignmentIndicator = (bits[index] & 0x04) >> 2;
		myPES.Copyright = (bits[index] & 0x02) >> 1;
		myPES.OriginalOrCopy = (bits[index] & 0x01);
		index += 1;
		myPES.PTSDTSFlags = (bits[index] & 0xc0) >> 6;
		myPES.ESCRFlag = (bits[index] & 0x20) >> 5;
		myPES.ESRateFlag = (bits[index] & 0x10) >> 4;
		myPES.DSMTrickModeFlag = (bits[index] & 0x08) >> 3;
		myPES.AdditionalCopyInfoFlag = (bits[index] & 0x04) >> 2;
		myPES.PESCRCFlag = (bits[index] & 0x02) >> 1;
		myPES.PESExtensionFlag = (bits[index] & 0x01);
		index += 1;
		myPES.PESHeaderDataLength = bits[index++];
		if (myPES.PTSDTSFlags == 0x2)
		{
			if ((bits[index] >> 4 & 0xF) != 0x2)
				return FALSE;
			INT64 timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
				(bits[index + 1] << 22) |
				((bits[index + 2] >> 1) << 15) |
				(bits[index + 3] << 7) |
				(bits[index + 4] >> 1);
			myPES.PTS = (((timestamp >> 33) & 0x7) << 30) | (((timestamp >> 17) & 0x7FFF) << 15) | (((timestamp >> 1) & 0x7FFF) << 0);
			myPES.DTS = 0;
			index += 5;
		}
		if (myPES.PTSDTSFlags == 0x3)
		{
			if ((bits[index] >> 4 & 0xF) != 0x3)
				return FALSE;
			INT64 timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
				(bits[index + 1] << 22) |
				((bits[index + 2] >> 1) << 15) |
				(bits[index + 3] << 7) |
				(bits[index + 4] >> 1);
			myPES.PTS = (((timestamp >> 33) & 0x7) << 30) | (((timestamp >> 17) & 0x7FFF) << 15) | (((timestamp >> 1) & 0x7FFF) << 0);
			index += 5;
			if ((bits[index] >> 4 & 0xF) != 0x1)
				return FALSE;
			timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
				(bits[index + 1] << 22) |
				((bits[index + 2] >> 1) << 15) |
				(bits[index + 3] << 7) |
				(bits[index + 4] >> 1);
			myPES.DTS = (((timestamp >> 33) & 0x7) << 30) | (((timestamp >> 17) & 0x7FFF) << 15) | (((timestamp >> 1) & 0x7FFF) << 0);
			index += 5;
		}
		if (myPES.ESCRFlag)
		{
			myPES.ESCR = (((UINT64)(bits[index] >> 3) & 0x7) << 30) |
				(UINT64)((bits[index] & 0x3) << 28) |
				(UINT64)(bits[index + 1] << 20) |
				(UINT64)((bits[index + 2] >> 3) << 15) |
				(UINT64)((bits[index + 2] & 0x03) << 13) |
				(UINT64)(bits[index + 3] << 5) |
				(UINT64)(bits[index + 4] >> 3);
			index += 6;
		}
		if (myPES.ESRateFlag)
		{
			myPES.ESRate = ((bits[index] & 0x7F) << 15) | (bits[index + 1] << 7) | (bits[index + 2] >> 1);
			index += 3;
		}
		if (myPES.DSMTrickModeFlag)
		{
			myPES.TrickModeControl = bits[index] >> 5;
			if (myPES.TrickModeControl == 0x0 || myPES.TrickModeControl == 0x3)
			{
				myPES.FieldID = (bits[index] >> 3) & 0x3;
				myPES.IntraSliceRefresh = (bits[index] >> 2) & 0x1;
				myPES.FrequencyTruncation = bits[index] & 0x3;
			}
			else if (myPES.TrickModeControl == 0x1 || myPES.TrickModeControl == 0x4)
			{
				myPES.RepCntrl = bits[index] & 0x1F;
			}
			else if (myPES.TrickModeControl == 0x2)
			{
				myPES.FieldID = (bits[index] >> 3) & 0x3;
			}
			index += 1;
		}
		if (myPES.AdditionalCopyInfoFlag)
		{
			myPES.AdditionalCopyInfo = bits[index++] & 0x7F;
		}
		if (myPES.PESCRCFlag)
		{
			myPES.PreviousPESPacketCRC = (bits[index] << 8) | bits[index + 1];
			index += 2;
		}
		if (myPES.PESExtensionFlag)
		{
			myPES.PESPrivateDataFlag = bits[index] >> 7;
			myPES.PackFieldLength = (bits[index] >> 6) & 0x1;
			myPES.ProgramPacketSequenceCounterFlag = (bits[index] >> 5) & 0x1;
			myPES.PSTDBufferFlag = (bits[index] >> 4) & 0x1;
			myPES.PESExtensionFlag2 = bits[index] & 0x1;
			index += 1;
			if (myPES.PESPrivateDataFlag)
			{
				index += 16;//16位
			}
			if (myPES.PackHeaderFieldFlag)
			{
				myPES.PackFieldLength = bits[index++];
				index += myPES.PackFieldLength;
			}
			if (myPES.ProgramPacketSequenceCounterFlag)
			{
				myPES.ProgramPacketSequenceCounter = bits[index] & 0x7F;
				myPES.MPEG1MPEG2Identifier = (bits[index + 1] >> 6) & 0x1;
				myPES.OriginalStuffLength = bits[index + 1] & 0x3F;
				index += 2;
			}
			if (myPES.PSTDBufferFlag)
			{
				myPES.PSTDBufferScale = (bits[index] >> 5) & 0x1;
				myPES.PSTDBufferSize = ((bits[index] & 0x1F) << 8) | bits[index + 1];
				index += 2;
			}
			if (myPES.PESExtensionFlag2)
			{
				myPES.PESExtensionFieldLength = bits[index++] & 0x7F;
				myPES.StreamIDExtensionFlag = bits[index] >> 7;
				if (myPES.StreamIDExtensionFlag)
				{
					myPES.StreamIDExtension = bits[index++] & 0x7F;
					index += myPES.PESExtensionFieldLength;
				}
			}
		}
		ASSERT(index == myPES.PESHeaderDataLength + 9);
		TSParser* parser = stream->GetParser();
		if (parser != NULL)
		{
			INT size = myPES.PESPacketLength - 3 - myPES.PESHeaderDataLength;
			ASSERT(size > 0);
			parser->Reset();
			parser->SetCapacity(size);
			size = TS_PACKET_SIZE - (offset + index);
			parser->Add((BYTE*)(bits + index), size);
		}
		return TRUE;
	}
	BOOL TSReader::ReadPacket(TS_PACKEG_HEADER& header, TS_BLOCK& block)
	{
		//TS传输包固定188个字节
		ZeroMemory(&block, sizeof(block));
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
				m_programs.RemoveAll();
				TS_PACKET_PAT myPAT;
				ZeroMemory(&myPAT, sizeof(myPAT));
				if (!ReadPAT(myPAT, m_programs, m_bits + index))
					return FALSE;
				m_versionNumber = myPAT.VersionNumber;
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
							if (stream->Slices >= 1)//处理h264
							{
								block.streamType = stream->StreamType;
								TSParser* parser = stream->GetParser();
								if (parser != NULL)
								{
									parser->Parse(block);
								}
							}
							TS_PACKET_PES myPES;
							ZeroMemory(&myPES, sizeof(myPES));
							if (!ReadPES(stream, myPES, block, m_bits + index, index))
								return FALSE;
							stream->Slices++;
						}
						else
						{
							TSParser* parser = stream->GetParser();
							if (parser != NULL)
							{
								INT size = TS_PACKET_SIZE - index;
								parser->Add((BYTE*)(m_bits + index), size);
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
						m_streams.RemoveAll();
						TS_PACKET_PMT myPMT;
						ZeroMemory(&myPMT, sizeof(myPMT));
						if (!ReadPTM(myPMT, m_streams, m_bits + index))
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
		{
			SAFE_DELETE(m_streams[i]);
		}
		m_streams.RemoveAll();
		m_programs.RemoveAll();
		m_stream.Release();
		m_versionNumber = -1;
		m_continuityCounter = -1;
		ZeroMemory(m_bits, TS_PACKET_SIZE);
		return TRUE;
	}
}

