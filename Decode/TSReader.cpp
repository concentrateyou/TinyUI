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
	static const BYTE PROGRAM_STREAM_MAP = 0xBC;
	static const BYTE PADDING_STREAM = 0xBE;
	static const BYTE PRIVATE_STREAM_2 = 0xBF;
	static const BYTE ECM_STREAM = 0xF0;
	static const BYTE EMM_STREAM = 0xF1;
	static const BYTE PROGRAM_STREAM_DIRECTORY = 0xFF;
	static const BYTE DSMCC_STREAM = 0xF2;
	static const BYTE H222_TYPE_E = 0xF8;
	static const BYTE E_STREAM = 0xF8;
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
	//////////////////////////////////////////////////////////////////////////
	TSH264Parser::TSH264Parser(ConfigCallback& callback)
		:m_parser(callback)
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
		TRACE("H264 PTS:%lld\n", block.pts);
		H264NALU sNALU;
		if (m_parser.Parse(data(), size(), sNALU))
		{
			block.video.codeType = sNALU.sliceType;
			block.video.type = sNALU.type;
			block.video.size = sNALU.size;
			block.video.data = new BYTE[block.video.size];
			memcpy_s(block.video.data, block.video.size, sNALU.bits, block.video.size);
		}
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	static INT AAC_Sample_Rates[16] =
	{
		96000, 88200, 64000, 48000, 44100, 32000,
		24000, 22050, 16000, 12000, 11025, 8000, 7350,
		0, 0, 0
	};
	TSAACParser::TSAACParser(ConfigCallback& callback)
		:m_callback(callback),
		m_timestamp(0.0F)
	{

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
		TRACE("AAC PTS:%lld\n", block.pts);
		block.audio.size = size();
		block.audio.data = new BYTE[block.audio.size];
		memcpy_s(block.audio.data, block.audio.size, data(), block.audio.size);
		return ParseADTS(block.audio.data, block.audio.size);
	}
	BOOL TSAACParser::ParseADTS(BYTE* bits, LONG size)
	{
		TinyBitReader reader;
		for (INT i = 0; i < (size - ADTS_HEADER_MIN_SIZE); i++)
		{
			BYTE* myP = &bits[i];
			if ((myP[0] != 0xFF) || ((myP[1] & 0xF6) != 0xF0))
				continue;
			INT rawsize = ((static_cast<int>(myP[5]) >> 5) | (static_cast<int>(myP[4]) << 3) | ((static_cast<int>(myP[3]) & 0x3) << 11));
			if (rawsize < ADTS_HEADER_MIN_SIZE)
				continue;
			INT remaining = size - i;
			if (remaining < rawsize)
			{
				continue;
			}
			if ((remaining >= rawsize + 2) && (myP[rawsize] != 0xFF) || ((myP[rawsize + 1] & 0xF6) != 0xF0))
			{
				continue;
			}
			BYTE layer = 0;
			BYTE profile = 0;
			BYTE channels = 0;
			BYTE samplesPerSec = 0;
			reader.Initialize(myP, 9);
			reader.SkipBits(13);
			reader.ReadBits(2, &layer);
			reader.SkipBits(1);
			reader.ReadBits(2, &profile);
			reader.ReadBits(4, &samplesPerSec);
			reader.SkipBits(1);
			reader.ReadBits(3, &channels);
			ASSERT(layer == 0);
			AACAudioConfig config(CodecAAC, static_cast<WORD>(channels), static_cast<DWORD>(samplesPerSec), 16);
			if (config != m_lastConfig)
			{
				m_lastConfig = config;
				if (!m_callback.IsNull())
				{
					vector<BYTE> buffer;
					buffer.resize(2);
					buffer[0] = ((profile + 1) << 3 | ((samplesPerSec & 0xE) >> 1));
					buffer[1] = (((samplesPerSec & 0x1) << 7) | (channels << 3));
					m_callback(&buffer[0], buffer.size(), TS_STREAM_TYPE_AUDIO_AAC, this);
				}
			}
		}
		return TRUE;
	}
	void TSAACParser::ParseAAC(TS_BLOCK& block, TinyLinkList<TS_BLOCK>& audios)
	{
		static FLOAT AACTimestamp = 1024.0F * 1000 / 44100;
		INT index = 0;
		TinyBitReader reader;
		for (INT i = 0; i < (block.audio.size - ADTS_HEADER_MIN_SIZE); i++)
		{
			BYTE* myP = &block.audio.data[i];
			if ((myP[0] != 0xFF) || ((myP[1] & 0xF6) != 0xF0))
				continue;
			INT rawsize = ((static_cast<INT>(myP[5]) >> 5) | (static_cast<INT>(myP[4]) << 3) | ((static_cast<INT>(myP[3]) & 0x3) << 11));
			if (rawsize < ADTS_HEADER_MIN_SIZE)
				continue;
			INT remaining = block.audio.size - i;
			if (remaining < rawsize)
			{
				continue;
			}
			if ((remaining >= rawsize + 2) && (myP[rawsize] != 0xFF) || ((myP[rawsize + 1] & 0xF6) != 0xF0))
			{
				continue;
			}
			BYTE layer = 0;
			BYTE absent = 0;
			reader.Initialize(myP, 9);//最多9字节 7+CRC
			reader.SkipBits(13);
			reader.ReadBits(2, &layer);
			reader.ReadBits(1, &absent);
			ASSERT(layer == 0);
			INT offset = (absent == 1 ? 7 : 9);
			TS_BLOCK myAudio = block;
			myAudio.pts = block.pts + (index * static_cast<LONGLONG>(AACTimestamp));
			myAudio.dts = myAudio.pts;
			myAudio.audio.size = rawsize - offset;
			myAudio.audio.data = new BYTE[myAudio.audio.size];
			memcpy_s(myAudio.audio.data, myAudio.audio.size, myP + offset, myAudio.audio.size);
			audios.InsertLast(myAudio);
			index += 1;
		}
		SAFE_DELETE_ARRAY(block.audio.data);
	}
	//////////////////////////////////////////////////////////////////////////
	TS_PACKET_STREAM::TS_PACKET_STREAM()
		:PTS(-1),
		DTS(-1)
	{

	}
	TS_PACKET_STREAM::~TS_PACKET_STREAM()
	{

	}
	TSParser* TS_PACKET_STREAM::GetParser(ConfigCallback& callback)
	{
		if (!m_parser)
		{
			if (StreamType == TS_STREAM_TYPE_AUDIO_AAC)
			{
				m_parser.Reset(new TSAACParser(callback));
			}
			if (StreamType == TS_STREAM_TYPE_VIDEO_H264)
			{
				m_parser.Reset(new TSH264Parser(callback));
			}
		}
		return m_parser;
	}
	BOOL TS_PACKET_STREAM::operator == (const TS_PACKET_STREAM& other)
	{
		return ElementaryPID == other.ElementaryPID;
	}
	//////////////////////////////////////////////////////////////////////////
	TSReader::TSReader()
		:m_versionNumber(-1),
		m_continuityCounter(-1),
		m_size(0),
		m_original(NULL)
	{
		ZeroMemory(m_bits, TS_PACKET_SIZE);
	}

	TSReader::~TSReader()
	{
	}
	void TSReader::SetConfigCallback(ConfigCallback&& callback)
	{
		m_configCallback = std::move(callback);
	}
	BOOL TSReader::OpenFile(LPCSTR pzFile)
	{
		if (!pzFile)
			return FALSE;
		this->Close();
		HRESULT hRes = SHCreateStreamOnFileA(pzFile, STGM_READ | STGM_FAILIFTHERE, &m_streamIO);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL TSReader::ReadBlock(TS_BLOCK& block)
	{
		ZeroMemory(&block, sizeof(block));
		for (;;)
		{
			if (m_audios.GetSize() > 0)
			{
				ITERATOR s = m_audios.First();
				TS_BLOCK& audio = m_audios.GetAt(s);
				memcpy_s(&block, sizeof(TS_BLOCK), &audio, sizeof(TS_BLOCK));
				block.audio.size = audio.audio.size;
				block.audio.data = new BYTE[block.audio.size];
				memcpy_s(block.audio.data, block.audio.size, audio.audio.data, block.audio.size);
				m_audios.RemoveAt(s);
				break;
			}
			TS_PACKEG_HEADER header;
			if (!ReadPacket(header, block))
				return FALSE;
			if (block.streamType > 0)
			{
				if (block.streamType == TS_STREAM_TYPE_AUDIO_AAC)//音频可能包括多帧,视频只有一帧
				{
					TSAACParser::ParseAAC(block, m_audios);
					if (m_audios.GetSize() > 0)
					{
						ITERATOR s = m_audios.First();
						TS_BLOCK& audio = m_audios.GetAt(s);
						memcpy_s(&block, sizeof(TS_BLOCK), &audio, sizeof(TS_BLOCK));
						block.audio.size = audio.audio.size;
						block.audio.data = new BYTE[block.audio.size];
						memcpy_s(block.audio.data, block.audio.size, audio.audio.data, block.audio.size);
						m_audios.RemoveAt(s);
					}
				}
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
			myPES.PTS = timestamp > 0 ? timestamp / 90 : timestamp;
			myPES.DTS = 0;
			stream->PTS = myPES.PTS;
			stream->DTS = myPES.DTS;
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
			myPES.PTS = timestamp > 0 ? timestamp / 90 : timestamp;
			index += 5;
			if ((bits[index] >> 4 & 0xF) != 0x1)
				return FALSE;
			timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
				(bits[index + 1] << 22) |
				((bits[index + 2] >> 1) << 15) |
				(bits[index + 3] << 7) |
				(bits[index + 4] >> 1);
			myPES.DTS = timestamp > 0 ? timestamp / 90 : timestamp;
			stream->PTS = myPES.PTS;
			stream->DTS = myPES.DTS;
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
		TSParser* parser = stream->GetParser(std::move(m_configCallback));
		if (parser != NULL)
		{
			INT size = myPES.PESPacketLength - 3 - myPES.PESHeaderDataLength;
			ASSERT(size > 0);
			parser->Reset();
			parser->SetCapacity(size);
			size = TS_PACKET_SIZE - (offset + index);
			parser->Add((BYTE*)(bits + index), size);
			m_size += size;
		}
		return TRUE;
	}
	BOOL TSReader::ReadPacket(TS_PACKEG_HEADER& header, TS_BLOCK& block)
	{
		BOOL loading = FALSE;
		//TS传输包固定188个字节
		ZeroMemory(&block, sizeof(block));
		ZeroMemory(&header, sizeof(header));
		ULONG ls = 0;
		HRESULT hRes = S_OK;
		hRes = m_streamIO->Read(m_bits, TS_PACKET_SIZE, &ls);
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
					TS_PACKET_STREAM* current = m_streams[i];
					if (current->ElementaryPID == header.PID)
					{
						if (header.PayloadUnitStartIndicator)
						{
							if (m_size != 0)
							{
								if (m_original != NULL)
								{
									block.pts = m_original->PTS;
									block.dts = m_original->DTS;
									block.streamType = m_original->StreamType;
									TSParser* parser = m_original->GetParser(std::move(m_configCallback));
									if (parser != NULL)
									{
										if (!parser->Parse(block))
											return FALSE;
									}
								}
								m_size = 0;
							}
							TS_PACKET_PES myPES;
							ZeroMemory(&myPES, sizeof(myPES));
							if (!ReadPES(current, myPES, block, m_bits + index, index))
								return FALSE;
							m_original = current;
						}
						else
						{
							ASSERT(m_original == current);
							TSParser* parser = current->GetParser(std::move(m_configCallback));
							if (parser != NULL)
							{
								INT size = TS_PACKET_SIZE - index;
								parser->Add((BYTE*)(m_bits + index), size);
								m_size += size;
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
		m_original = NULL;
		for (INT i = 0;i < m_streams.GetSize();i++)
		{
			SAFE_DELETE(m_streams[i]);
		}
		m_streams.RemoveAll();
		m_programs.RemoveAll();
		m_streamIO.Release();
		m_versionNumber = -1;
		m_continuityCounter = -1;
		ZeroMemory(m_bits, TS_PACKET_SIZE);
		return TRUE;
	}
}

