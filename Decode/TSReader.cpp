#include "stdafx.h"
#include "TSReader.h"

namespace Decode
{
	/// <summary>
	/// CRC32检验
	/// </summary>
	static BOOL CheckCRC32(const BYTE* bits, INT size)
	{
		UINT32 crc = 0xFFFFFFFFu;
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
	{

	}
	TSParser::~TSParser()
	{

	}
	void TSParser::Add(BYTE* bits, INT size)
	{
		m_io.Add(bits, size);
	}
	void TSParser::Remove(INT size)
	{
		m_io.Remove(0, size);
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
	TS_ERROR TSH264Parser::Parse(TS_BLOCK& block, Timing& timing)
	{
		if (!m_timings.Lookup(timing, NULL))
		{
			m_timings.InsertLast(timing);
		}
		H264NALU sNALU;
		TS_ERROR iRes = m_parser.Parse(data(), size(), sNALU);
		if (iRes != TS_ERROR_NONE)
		{
			ZeroMemory(&block, sizeof(block));
		}
		else
		{
			ITERATOR s = m_timings.First();
			if (s != NULL)
			{
				Timing& timing = m_timings.GetAt(s);
				block.pts = timing.PTS;
				block.dts = timing.DTS;
				m_timings.RemoveAt(s);
			}
			block.video.codeType = sNALU.sliceType;
			block.video.type = sNALU.type;
			block.video.size = sNALU.size;
			block.video.data = new BYTE[block.video.size];
			memcpy_s(block.video.data, block.video.size, sNALU.bits, block.video.size);
			const BYTE* ps = sNALU.bits + sNALU.size;
			this->Remove(ps - data());
		}
		return iRes;
	}
	/// <summary>
	/// 是否ADTS同步头
	/// </summary>
	static BOOL IsADTSSyncword(const BYTE* bits)
	{
		return (bits[0] == 0xFF) && ((bits[1] & 0xF6) == 0xF0);
	}

	TSAACParser::TSAACParser(ConfigCallback& callback)
		:m_callback(callback)
	{

	}
	TSAACParser::~TSAACParser()
	{

	}
	BYTE TSAACParser::GetStreamType() const
	{
		return TS_STREAM_TYPE_AUDIO_AAC;
	}
	const AACAudioConfig& TSAACParser::GetAudioConfig() const
	{
		return m_lastConfig;
	}
	TS_ERROR TSAACParser::Parse(TS_BLOCK& block, Timing& timing)
	{
		if (!m_timings.Lookup(timing, NULL))
		{
			m_timings.InsertLast(timing);
		}
		block.audio.size = size();
		block.audio.data = new BYTE[block.audio.size];
		memcpy_s(block.audio.data, block.audio.size, data(), block.audio.size);
		if (ParseADTS(block.audio.data, block.audio.size))
		{
			ITERATOR s = m_timings.First();
			if (s != NULL)
			{
				Timing& timing = m_timings.GetAt(s);
				block.pts = timing.PTS;
				block.dts = timing.DTS;
				m_timings.RemoveAt(s);
			}
			this->Remove(block.audio.size);
			return TS_ERROR_NONE;
		}
		return TS_ERROR_UNKNOW;
	}
	BOOL TSAACParser::ParseADTS(BYTE* bits, LONG size)
	{
		TinyBitReader reader;
		for (INT i = 0; i < (size - ADTS_HEADER_MIN_SIZE); i++)
		{
			BYTE* myP = &bits[i];
			if (!IsADTSSyncword(myP))
				continue;
			INT rawsize = ((static_cast<int>(myP[5]) >> 5) | (static_cast<int>(myP[4]) << 3) | ((static_cast<int>(myP[3]) & 0x3) << 11));
			if (rawsize < ADTS_HEADER_MIN_SIZE)
				continue;
			INT remaining = size - i;
			if (remaining < rawsize)
				continue;
			if ((remaining >= rawsize + 2) && !IsADTSSyncword(&myP[rawsize]))
				continue;
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
				ASSERT(samplesPerSec >= 0 && samplesPerSec < 16);
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
	void TSAACParser::ParseAAC(TS_BLOCK& block, TinyLinkList<TS_BLOCK>& audios, DOUBLE AACTimestamp)
	{
		DOUBLE timestamp = 0.0F;
		TinyBitReader reader;
		for (INT i = 0; i < (block.audio.size - ADTS_HEADER_MIN_SIZE); i++)
		{
			BYTE* myP = &block.audio.data[i];
			if (!IsADTSSyncword(myP))
				continue;
			INT rawsize = ((static_cast<INT>(myP[5]) >> 5) | (static_cast<INT>(myP[4]) << 3) | ((static_cast<INT>(myP[3]) & 0x3) << 11));
			if (rawsize < ADTS_HEADER_MIN_SIZE)
				continue;
			INT remaining = block.audio.size - i;
			if (remaining < rawsize)
				continue;
			if ((remaining >= (rawsize + 2)) && !IsADTSSyncword(&myP[rawsize]))
				continue;
			BYTE layer = 0;
			BYTE absent = 0;
			reader.Initialize(myP, 9);//最多9字节 7+CRC
			reader.SkipBits(13);
			reader.ReadBits(2, &layer);
			reader.ReadBits(1, &absent);
			ASSERT(layer == 0);
			INT offset = (absent == 1 ? 7 : 9);
			TS_BLOCK myAudio = block;
			myAudio.pts = block.pts + timestamp;
			myAudio.dts = myAudio.pts;
			myAudio.audio.size = rawsize - offset;
			myAudio.audio.data = new BYTE[myAudio.audio.size];
			memcpy_s(myAudio.audio.data, myAudio.audio.size, myP + offset, myAudio.audio.size);
			audios.InsertLast(myAudio);
			timestamp += AACTimestamp;
		}
		SAFE_DELETE_ARRAY(block.audio.data);
	}
	//////////////////////////////////////////////////////////////////////////
	TS_PACKET_STREAM::TS_PACKET_STREAM(ConfigCallback&& callback)
		:PTS(NAN),
		DTS(NAN),
		m_configCallback(std::move(callback))
	{

	}
	TS_PACKET_STREAM::~TS_PACKET_STREAM()
	{

	}
	TSParser* TS_PACKET_STREAM::GetParser()
	{
		if (m_parser != NULL)
			return m_parser;
		if (StreamType == TS_STREAM_TYPE_AUDIO_AAC)
			m_parser.Reset(new TSAACParser(m_configCallback));
		if (StreamType == TS_STREAM_TYPE_VIDEO_H264)
			m_parser.Reset(new TSH264Parser(m_configCallback));
		return m_parser;
	}
	BOOL TS_PACKET_STREAM::operator == (const TS_PACKET_STREAM& other)
	{
		return ElementaryPID == other.ElementaryPID;
	}
	//////////////////////////////////////////////////////////////////////////
	TSReader::TSReader()
		:m_versionNumberPAT(-1),
		m_versionNumberPMT(-1),
		m_previous(NULL),
		m_audioSR(0.0F)
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
	BOOL TSReader::OpenMemory(const BYTE* bits, LONG size)
	{
		if (!bits || size <= 0)
			return FALSE;
		this->Close();
		m_stream.Attach(new TinyMemoryStream());
		if (!m_stream)
			return FALSE;
		TinyMemoryStream* ps = static_cast<TinyMemoryStream*>(m_stream.Ptr());
		BYTE* s = ps->ReAlloc(size);
		ps->m_cbData = size;
		CopyMemory(ps->m_pData, bits, size);
		return TRUE;
	}
	BOOL TSReader::OpenFile(LPCSTR pzFile)
	{
		if (!PathFileExists(pzFile))
			return FALSE;
		this->Close();
		HRESULT hRes = SHCreateStreamOnFileA(pzFile, STGM_READ | STGM_FAILIFTHERE, &m_stream);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL TSReader::OpenURL(LPCSTR pzURL)
	{
		if (!PathIsURL(pzURL))
			return FALSE;
		this->Close();
		m_stream.Attach(new HTTPStream());
		if (!m_stream)
			return FALSE;
		HTTPStream* ps = static_cast<HTTPStream*>(m_stream.Ptr());
		if (!ps->Open(pzURL))
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
				m_audios.RemoveAt(s);
				break;
			}
			TS_PACKEG_HEADER header;
			if (!ReadPacket(header, block))
				return FALSE;
			if (block.streamType > 0)
			{
				if (block.streamType == TS_STREAM_TYPE_AUDIO_AAC)
				{
					TSAACParser::ParseAAC(block, m_audios, m_audioSR);
					if (m_audios.GetSize() > 0)
					{
						ITERATOR s = m_audios.First();
						TS_BLOCK& audio = m_audios.GetAt(s);
						memcpy_s(&block, sizeof(TS_BLOCK), &audio, sizeof(TS_BLOCK));
						m_audios.RemoveAt(s);
					}
				}
				break;
			}
		}
		return TRUE;
	}
	void TSReader::OnConfigCallback(const BYTE* bits, LONG size, BYTE streamType, LPVOID ps)
	{
		if (streamType == TS_STREAM_TYPE_AUDIO_AAC)
		{
			TSAACParser* aac = static_cast<TSAACParser*>(ps);
			if (aac != NULL)
			{
				m_audioSR = (1024.0F * 1000 / static_cast<DOUBLE>(aac->GetAudioConfig().GetSampleRate()));
			}
		}
		if (!m_configCallback.IsNull())
		{
			m_configCallback(bits, size, streamType, ps);
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
		if (myPAT.TableID != 0x0)
			return FALSE;
		myPAT.SectionSyntaxIndicator = bits[index] >> 7;
		if (myPAT.SectionSyntaxIndicator != 0x1)
			return FALSE;
		myPAT.Zero = bits[index] >> 6 & 0x1;
		if (myPAT.Zero != 0x0)
			return FALSE;
		myPAT.Reserved1 = bits[index] >> 4 & 0x3;
		myPAT.SectionLength = ((bits[index] & 0x0F) << 8) | bits[index + 1];
		if (myPAT.SectionLength < 5 || myPAT.SectionLength > 1021)
			return FALSE;
		index += 2;
		myPAT.TransportStreamID = ((bits[index] << 8) | bits[index + 1]);
		index += 2;
		myPAT.Reserved2 = bits[index] >> 6;
		myPAT.VersionNumber = (bits[index] >> 1) & 0x1F;
		myPAT.CurrentNextIndicator = (bits[index] << 7) >> 7;
		//PAT当前有效无效则忽略
		if (myPAT.CurrentNextIndicator != 0x1)
			return TRUE;
		myPAT.SectionNumber = bits[++index];
		myPAT.LastSectionNumber = bits[++index];
		INT size = myPAT.SectionLength;
		myPAT.CRC32 = (bits[size - 1] & 0x000000FF) << 24 | (bits[size] & 0x000000FF) << 16 | (bits[size + 1] & 0x000000FF) << 8 | (bits[size + 2] & 0x000000FF);
		if (!CheckCRC32(bits, size + 3))
			return FALSE;
		//PAT没有改变
		if (m_versionNumberPAT == myPAT.VersionNumber)
			return TRUE;
		m_versionNumberPAT = myPAT.VersionNumber;
		m_programs.RemoveAll();
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
	BOOL TSReader::ReadPMT(TS_PACKET_PMT& myPMT, TinyArray<TinyScopedReferencePtr<TS_PACKET_STREAM>>& streams, const BYTE* bits)
	{
		INT index = 0;
		myPMT.TableID = bits[index++];
		if (myPMT.TableID != 0x2)
			return FALSE;
		myPMT.SectionSyntaxIndicator = bits[index] >> 7;
		if (myPMT.SectionSyntaxIndicator != 0x1)
			return FALSE;
		myPMT.Zero = bits[index] >> 6 & 0x1;
		myPMT.Reserved1 = bits[index] >> 4 & 0x3;
		if (myPMT.Zero != 0x0)
			return FALSE;
		myPMT.SectionLength = ((bits[index] & 0x0F) << 8) | bits[index + 1];
		if (myPMT.SectionLength < 5 || myPMT.SectionLength > 1021)
			return FALSE;
		index += 2;
		myPMT.ProgramNumber = bits[index] << 8 | bits[index + 1];
		index += 2;
		myPMT.Reserved2 = bits[index] >> 6;
		myPMT.VersionNumber = bits[index] >> 1 & 0x1F;
		myPMT.CurrentNextIndicator = (bits[index] << 7) >> 7;
		//PMT当前有效
		if (myPMT.CurrentNextIndicator != 0x1)
			return FALSE;
		myPMT.SectionNumber = bits[++index];
		if (myPMT.SectionNumber != 0)
			return FALSE;
		myPMT.LastSectionNumber = bits[++index];
		if (myPMT.LastSectionNumber != 0)
			return FALSE;
		index += 1;
		myPMT.Reserved3 = bits[index] >> 5;
		myPMT.PCR_PID = ((bits[index] << 8) | bits[index + 1]) & 0x1FFF;
		index += 2;
		myPMT.Reserved4 = bits[index] >> 4;
		myPMT.ProgramInfoLength = (bits[index] & 0x0F) << 8 | bits[index + 1];
		index += 2;
		index += myPMT.ProgramInfoLength;
		INT size = myPMT.SectionLength;
		myPMT.CRC32 = (bits[myPMT.SectionLength - 1] & 0x000000FF) << 24 | (bits[myPMT.SectionLength] & 0x000000FF) << 16 | (bits[myPMT.SectionLength + 1] & 0x000000FF) << 8 | (bits[myPMT.SectionLength + 2] & 0x000000FF);
		if (!CheckCRC32(bits, myPMT.SectionLength + 3))
			return FALSE;
		if (m_versionNumberPMT == myPMT.VersionNumber)
			return TRUE;
		m_versionNumberPMT = myPMT.VersionNumber;
		streams.RemoveAll();
		for (;;)
		{
			if (index >= (size - 4))
				break;
			TS_PACKET_STREAM* stream = new TS_PACKET_STREAM(BindCallback(&TSReader::OnConfigCallback, this));
			if (!stream)
				return FALSE;
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
	BOOL TSReader::ReadPES(TS_PACKET_STREAM* stream, TS_PACKET_PES& myPES, const BYTE* bits, INT offset, BOOL bPayloadUnitStartIndicator)
	{
		ASSERT(stream);
		INT index = 0;
		if (bPayloadUnitStartIndicator)
		{
			myPES.PacketStartCodePrefix = bits[index] << 16 | bits[index + 1] << 8 | bits[index + 2];
			if (myPES.PacketStartCodePrefix != 0x000001)
				return FALSE;
			index += 3;
			myPES.StreamID = bits[index++];
			myPES.PESPacketLength = bits[index] << 8 | bits[index + 1];//可以为0
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
				myPES.PTS = timestamp;
				myPES.DTS = timestamp;
				stream->PTS = myPES.PTS > 0 ? myPES.PTS / 90.0F : myPES.PTS;
				stream->DTS = stream->PTS;
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
				myPES.PTS = timestamp;
				index += 5;
				if ((bits[index] >> 4 & 0xF) != 0x1)
					return FALSE;
				timestamp = ((((INT64)bits[index] >> 1) & 0x7) << 30) |
					(bits[index + 1] << 22) |
					((bits[index + 2] >> 1) << 15) |
					(bits[index + 3] << 7) |
					(bits[index + 4] >> 1);
				myPES.DTS = timestamp;
				stream->PTS = myPES.PTS > 0 ? myPES.PTS / 90.0F : myPES.PTS;
				stream->DTS = myPES.DTS > 0 ? myPES.DTS / 90.0F : myPES.DTS;
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
		}
		TSParser* parser = stream->GetParser();
		if (parser != NULL)
		{
			parser->Add((BYTE*)(bits + index), TS_PACKET_SIZE - (offset + index));
		}
		return TRUE;
	}
	BOOL TSReader::ReadSDT(TS_PACKET_SDT& mySDT, const BYTE* bits)
	{
		INT index = 0;
		mySDT.TableID = bits[index++];
		if ((mySDT.TableID != 0x42) || (mySDT.TableID > 0x46))
			return FALSE;
		mySDT.SectionSyntaxIndicator = bits[index] >> 7;
		if (mySDT.SectionSyntaxIndicator != 0x1)
			return FALSE;
		mySDT.Reserved1 = bits[index] >> 6 & 0x1;
		mySDT.Reserved2 = bits[index] >> 4 & 0x3;
		mySDT.SectionLength = ((bits[index] & 0x0F) << 8) | bits[index + 1];
		if (mySDT.SectionLength < 5 || mySDT.SectionLength > 1021)
			return FALSE;
		index += 2;
		mySDT.TransportStreamID = ((bits[index] << 8) | bits[index + 1]);
		index += 2;
		mySDT.Reserved3 = bits[index] >> 6;
		mySDT.VersionNumber = (bits[index] >> 1) & 0x1F;
		mySDT.CurrentNextIndicator = (bits[index] << 7) >> 7;
		mySDT.SectionNumber = bits[++index];
		mySDT.LastSectionNumber = bits[++index];
		index += 1;
		mySDT.OriginalNetwordID = ((bits[index] & 0x0F) << 8) | bits[index + 1];
		index += 2;
		mySDT.Reserved4 = bits[++index];
		INT size = mySDT.SectionLength;
		mySDT.CRC32 = (bits[size - 1] & 0x000000FF) << 24 | (bits[size] & 0x000000FF) << 16 | (bits[size - 1] & 0x000000FF) << 8 | (bits[size - 2] & 0x000000FF);
		if (!CheckCRC32(bits, size + 3))//CRC32校验
			return FALSE;
		for (INT i = 0;i < size - 12;)
		{
			TS_PACKET_SDT::TS_PACKET_SERVICE* ps = new TS_PACKET_SDT::TS_PACKET_SERVICE();
			if (!ps)
				return FALSE;
			mySDT.Services.Add(ps);
			ps->ServiceID = bits[index] << 8 | bits[index + 1];
			index += 2;
			ps->Reserved = (bits[index] & 0x1C) >> 2;
			ps->EITScheduleFlag = (bits[index] & 0x02) >> 1;
			ps->EITPresentFollowingFlag = bits[index] & 0x01;
			index += 1;
			ps->RunningStatus = (bits[index] & 0xe0) >> 5;
			ps->FreeCAMode = (bits[index] & 0x10) >> 4;
			ps->DescriptorsLoopLength = (bits[index] & 0x0F) << 8 | bits[index + 1];
			index += 2;
			INT offset = 0;
			while (offset < ps->DescriptorsLoopLength)
			{
				BYTE descriptorTag = bits[index++];
				BYTE descriptorSize = bits[index++];
				offset += (descriptorSize + 2);
				switch (descriptorTag)
				{
				case 0x48:
				{
					TS_PACKET_DESCRIPTOR* val = new TS_PACKET_SERVICE_DESCRIPTOR();
					val->DescriptorTag = descriptorTag;
					val->DescriptorLength = descriptorSize;
					if (val->Parser(bits + index, descriptorSize))
					{
						ps->Descriptors.Add(val);
					}
					else
					{
						SAFE_DELETE(val);
					}
				}
				break;
				}
			}
			ASSERT(offset == ps->DescriptorsLoopLength);
			index += ps->DescriptorsLoopLength;
			i += (offset + 5);
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
		INT continuityCounter = header.ContinuityCounter;
		INT* val = m_continuityCounterMap.GetValue(header.PID);
		if (val != NULL)
		{
			continuityCounter = *val;
			continuityCounter = (continuityCounter + 1) % 16;
			if (continuityCounter != header.ContinuityCounter)
			{
				TRACE("Invalid continuityCounter:%d,PID:%d%n", continuityCounter, header.PID);
			}
		}
		if (header.PID == 0)
		{
			INT a = 0;
		}
		m_continuityCounterMap.SetAt(header.PID, continuityCounter);
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
				TS_PACKET_PAT myPAT;
				ZeroMemory(&myPAT, sizeof(myPAT));
				if (!ReadPAT(myPAT, m_programs, m_bits + index))
					return FALSE;
			}
			if (header.PID >= 0x0010)
			{
				//STD
				if (header.PID == 0x0011)
				{
					if (header.PayloadUnitStartIndicator)//表示带有pointer_field
					{
						INT pointer = m_bits[index++] & 0x0F;//有效载荷的位置
						index += pointer;
					}
					TS_PACKET_SDT mySTD;
					ZeroMemory(&mySTD, sizeof(mySTD));
					if (!ReadSDT(mySTD, m_bits + index))
						return FALSE;
				}
				//PES
				for (INT i = 0;i < m_streams.GetSize();i++)
				{
					TS_PACKET_STREAM* current = m_streams[i].Ptr();
					if (current->ElementaryPID == header.PID)
					{
						if (header.PayloadUnitStartIndicator)
						{
							if (m_previous != NULL)
							{
								block.streamType = m_previous->StreamType;
								TSParser* parser = m_previous->GetParser();
								if (parser != NULL)
								{
									Timing timing = { m_previous->PTS,m_previous->DTS };
									TS_ERROR iRes = parser->Parse(block, timing);
									if (iRes != TS_ERROR_NONE &&
										iRes != TS_ERROR_NEED_MORE)
									{
										return FALSE;
									}
								}
							}
							m_previous = current;
						}
						TS_PACKET_PES myPES;
						ZeroMemory(&myPES, sizeof(myPES));
						if (!ReadPES(current, myPES, m_bits + index, index, header.PayloadUnitStartIndicator))
							return FALSE;
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
						TS_PACKET_PMT myPMT;
						ZeroMemory(&myPMT, sizeof(myPMT));
						if (!ReadPMT(myPMT, m_streams, m_bits + index))
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
		m_previous = NULL;
		m_continuityCounterMap.RemoveAll();
		m_streams.RemoveAll();
		m_programs.RemoveAll();
		m_stream.Release();
		ITERATOR s = m_audios.First();
		while (s != NULL)
		{
			TS_BLOCK& audio = m_audios.GetAt(s);
			SAFE_DELETE_ARRAY(audio.audio.data);
			s = m_audios.Next(s);
		}
		m_audios.RemoveAll();
		m_versionNumberPAT = -1;
		m_versionNumberPMT = -1;
		ZeroMemory(m_bits, TS_PACKET_SIZE);
		return TRUE;
	}
}

