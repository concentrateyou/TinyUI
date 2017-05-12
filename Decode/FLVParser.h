#pragma once
#include "Utility.h"
#include "AACDecode.h"
#include "MPG123Decode.h"
#include "H264Decode.h"
#include <vector>

using namespace std;
using namespace TinyUI;

namespace Decode
{
#define TYPE_ONTEXTDATA 1
#define TYPE_ONCAPTION 2
#define TYPE_ONCAPTIONINFO 3
#define TYPE_UNKNOWN 9

	typedef struct tagFLV_HEADER
	{
		BYTE signature[3];
		BYTE version;
		BYTE streamType;
		BYTE offset[4];
	}FLV_HEADER;

	typedef struct tagFLV_TAG_HEADER
	{
		BYTE type;
		BYTE size[3];
		BYTE timestamp[3];
		BYTE timestampex;
		BYTE streamID[3];
	} FLV_TAG_HEADER;

	typedef struct tagFLV_TAG_VIDEO
	{
		BYTE codeID : 4;//编码ID
		BYTE codeType : 4;//帧类型
	}FLV_TAG_VIDEO;

	typedef struct tagFLV_TAG_AUDIO
	{
		BYTE channel : 1;
		BYTE bitsPerSample : 1;
		BYTE samplesPerSec : 2;
		BYTE codeID : 4;
	}FLV_TAG_AUDIO;

	enum FLV_SAMPLERATE
	{
		FLV_SAMPLERATE_SPECIAL = 0,
		FLV_SAMPLERATE_11025HZ = 1,
		FLV_SAMPLERATE_22050HZ = 2,
		FLV_SAMPLERATE_44100HZ = 3
	};

	enum FLV_AUDIO_CODECID
	{
		FLV_CODECID_PCM = 0,
		FLV_CODECID_ADPCM = 1,
		FLV_CODECID_MP3 = 2,
		FLV_CODECID_PCM_LE = 3,
		FLV_CODECID_NELLYMOSER_16KHZ_MONO = 4,
		FLV_CODECID_NELLYMOSER_8KHZ_MONO = 5,
		FLV_CODECID_NELLYMOSER = 6,
		FLV_CODECID_PCM_ALAW = 7,
		FLV_CODECID_PCM_MULAW = 8,
		FLV_CODECID_AAC = 10,
		FLV_CODECID_SPEEX = 11
	};

	enum FLV_VIDEO_CODECID
	{
		FLV_CODECID_H263 = 2,
		FLV_CODECID_SCREEN = 3,
		FLV_CODECID_VP6 = 4,
		FLV_CODECID_VP6A = 5,
		FLV_CODECID_SCREEN2 = 6,
		FLV_CODECID_H264 = 7,
		FLV_CODECID_REALH263 = 8,
		FLV_CODECID_MPEG4 = 9
	};

	enum FLV_FRAME_TYPE
	{
		FLV_FRAME_KEY = 1,
		FLV_FRAME_INTER = 2,
		FLV_FRAME_DISP_INTER = 3,
		FLV_FRAME_GENERATED_KEY = 4,
		FLV_FRAME_VIDEO_INFO_CMD = 5
	};

	enum FLV_PACKET_TYPE
	{
		FLV_AVCDecoderConfigurationRecord = 0,
		FLV_NALU = 1,
		FLV_AudioSpecificConfig = 2,
		FLV_AACRaw = 3
	};

	enum FLV_TAG_TYPE
	{
		FLV_AUDIO = 0x08,
		FLV_VIDEO = 0x09,
		FLV_SCRIPT = 0x12
	};

	enum AMFDataType
	{
		AMF_DATA_TYPE_NUMBER = 0x00,
		AMF_DATA_TYPE_BOOL = 0x01,
		AMF_DATA_TYPE_STRING = 0x02,
		AMF_DATA_TYPE_OBJECT = 0x03,
		AMF_DATA_TYPE_NULL = 0x05,
		AMF_DATA_TYPE_UNDEFINED = 0x06,
		AMF_DATA_TYPE_REFERENCE = 0x07,
		AMF_DATA_TYPE_MIXEDARRAY = 0x08,
		AMF_DATA_TYPE_OBJECT_END = 0x09,
		AMF_DATA_TYPE_ARRAY = 0x0a,
		AMF_DATA_TYPE_DATE = 0x0b,
		AMF_DATA_TYPE_LONG_STRING = 0x0c,
		AMF_DATA_TYPE_UNSUPPORTED = 0x0d,
	};

	typedef struct tagFLV_SCRIPTDATA
	{
		BOOL	hasAudio;
		BOOL	hasVideo;
		BOOL	hasMetadata;
		BOOL	hasKeyframes;
		BOOL	canSeekToEnd;
		BOOL	stereo;
		DOUBLE	duration;
		DOUBLE	filesize;
		DOUBLE	datasize;
		DOUBLE	audiosize;
		DOUBLE	videosize;
		DOUBLE	videocodecid;
		DOUBLE	audiocodecid;
		DOUBLE	audiodatarate;
		DOUBLE	audiosamplesize;
		DOUBLE	audiosamplerate;
		DOUBLE	videodatarate;
		DOUBLE	framerate;
		DOUBLE	width;
		DOUBLE	height;
		DOUBLE	lasttimestamp;
		DOUBLE	lastkeyframetimestamp;
		DOUBLE	lastkeyframelocation;
	}FLV_SCRIPTDATA;

	typedef struct tagAVCDecoderConfigurationRecord
	{
		BYTE	ConfigurationVersion;
		BYTE	AVCProfileIndication;
		BYTE	ProfileCompatibility;
		BYTE	AVCLevelIndication;
		BYTE	LengthSizeMinusOne;
		BYTE	NumOfSequenceParameterSets;
		BYTE	NumOfPictureParameterSets;
	}AVCDecoderConfigurationRecord;

	typedef struct tagFLV_PACKET
	{
		LONGLONG	dts;
		LONGLONG	pts;
		BYTE		codeID;//帧类型 h264,aac
		BYTE		codeType;//帧类型 
		BYTE		packetType;//包类型
		BYTE		channel;
		BYTE		bitsPerSample;
		BYTE		samplesPerSec;
	}FLV_PACKET;

	static const UINT32 H264StartCode = 0x01000000;
	/// <summary>
	/// FLV大端数据
	/// 目前音频支持支AAC,MP3和PCM,视频H.264
	/// </summary>
	class FLVParser
	{
		DISALLOW_COPY_AND_ASSIGN(FLVParser)
	public:
		FLVParser();
		~FLVParser();
		BOOL Open(LPCSTR pzFile);
		BOOL Parse();
		BOOL Close();
	public:
		Event<void(BYTE*, LONG, FLV_PACKET*)> EVENT_AUDIO;
		Event<void(BYTE*, LONG, FLV_PACKET*)> EVENT_VIDEO;
		Event<void(FLV_SCRIPTDATA*)> EVENT_SCRIPT;
	private:
		BOOL ParseVideo(BYTE* data, INT size);
		BOOL ParseAudio(BYTE* data, INT size);
		BOOL ParseScript(BYTE* data, INT size);
		BOOL ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size);
		BOOL ParseMPEG4(FLV_TAG_VIDEO* video, BYTE* data, INT size);
		BOOL ParseNALU(FLV_TAG_VIDEO* video, INT* cts, BYTE* data, INT size);
	private:
		BOOL							m_bAudio;
		BOOL							m_bVideo;
		BYTE							m_lengthSizeMinusOne;
		FILE*							m_hFile;
		LONGLONG						m_dts;
		LONG							m_index;
	};

	typedef struct tagFLV_BLOCK
	{
		BYTE		type;
		LONGLONG	dts;
		LONGLONG	pts;
		union
		{
			struct
			{
				BYTE*	data;
				LONG	size;
				BYTE	channel : 1;
				BYTE	bitsPerSample : 1;
				BYTE	samplesPerSec : 2;
				BYTE	codeID : 4;
				BYTE	packetType;
			}audio;
			struct
			{
				BYTE*	data;
				LONG	size;
				LONG	cts;
				BYTE	codeID : 4;//编码ID
				BYTE	codeType : 4;//帧类型
				BYTE	packetType;
			}video;
			struct
			{
				BOOL	hasAudio;
				BOOL	hasVideo;
				BOOL	hasMetadata;
				BOOL	hasKeyframes;
				BOOL	canSeekToEnd;
				BOOL	stereo;
				DOUBLE	duration;
				DOUBLE	filesize;
				DOUBLE	datasize;
				DOUBLE	audiosize;
				DOUBLE	videosize;
				DOUBLE	videocodecid;
				DOUBLE	audiocodecid;
				DOUBLE	audiodatarate;
				DOUBLE	audiosamplesize;
				DOUBLE	audiosamplerate;
				DOUBLE	videodatarate;
				DOUBLE	framerate;
				DOUBLE	width;
				DOUBLE	height;
				DOUBLE	lasttimestamp;
				DOUBLE	lastkeyframetimestamp;
				DOUBLE	lastkeyframelocation;
			}script;
		};
	}FLV_BLOCK;


	/// <summary>
	/// FLV读取器
	/// </summary>
	class FLVReader
	{
		DISALLOW_COPY_AND_ASSIGN(FLVReader)
	public:
		FLVReader();
		~FLVReader();
		FLV_SCRIPTDATA GetScript();
		BOOL Open(LPCSTR pzFile);
		BOOL ReadBlock(FLV_BLOCK& block);
		BOOL Close();
	private:
		BOOL ParseScript(BYTE* data, INT size, FLV_SCRIPTDATA& script);
		BOOL ParseVideo(BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseAudio(BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseMPEG4(FLV_TAG_VIDEO* video, BYTE* data, INT size, FLV_BLOCK& block);
		BOOL ParseNALU(FLV_TAG_VIDEO* video, FLV_BLOCK& block);
	private:
		FLV_SCRIPTDATA			m_script;
		ULONGLONG				m_offset;
		BOOL					m_bAudio;
		BOOL					m_bVideo;
		BYTE					m_lengthSizeMinusOne;
		TinyComPtr<IStream>		m_stream;
		TinyBufferArray<BYTE>	m_nalus;
		FLV_TAG_VIDEO			m_videoTag;
		LONGLONG				m_dts;
		BYTE*					m_naluPtr;
		INT						m_naluOffset;
	};
}

