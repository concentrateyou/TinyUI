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
		LONGLONG	timestamp;
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
	class FLVParse
	{
	public:
		FLVParse();
		~FLVParse();
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
		BOOL ParseNALU(FLV_TAG_VIDEO* video, BYTE* data, INT size);
	private:
		BOOL							m_bStop;
		BOOL							m_bAudio;
		BOOL							m_bVideo;
		BYTE							m_lengthSizeMinusOne;
		FILE*							m_hFile;
		LONGLONG						m_timestamps[2];
	};
}

