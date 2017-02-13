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

	enum FLV_VIDEO_FRAME_TYPE
	{
		FLV_FRAME_KEY = 1,
		FLV_FRAME_INTER = 2,
		FLV_FRAME_DISP_INTER = 3,
		FLV_FRAME_GENERATED_KEY = 4,
		FLV_FRAME_VIDEO_INFO_CMD = 5
	};

	typedef struct tagAVCDecoderConfigurationRecord
	{
		BYTE ConfigurationVersion;
		BYTE AVCProfileIndication;
		BYTE ProfileCompatibility;
		BYTE AVCLevelIndication;
		BYTE LengthSizeMinusOne;
	}AVCDecoderConfigurationRecord;

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

	/// <summary>
	/// FLV大端数据
	/// 目前音频支持支AAC,MP3和PCM,视频H.264
	/// </summary>
	class FLVFile
	{
	public:
		FLVFile();
		~FLVFile();
		BOOL Open(LPCSTR pzFile);
		BOOL Decode();
		BOOL Close();
	public:
		Event<void(BYTE*, LONG, LPVOID)> EVENT_AUDIO;
		Event<void(BYTE*, LONG, LPVOID)> EVENT_VIDEO;
	private:
		BOOL ParseVideo(BYTE* data, INT size);
		BOOL ParseAudio(BYTE* data, INT size);
		BOOL ParseScript(BYTE* data, INT size);
		BOOL ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size);
		BOOL ParseNALU(FLV_TAG_VIDEO* video, BYTE* data, INT size);
		void OnAudioDone(BYTE*, INT, LPVOID);
		void OnVideoDone(BYTE*, INT, LPVOID);
		using SPS = vector<BYTE>;
		using PPS = vector<BYTE>;
	private:
		BOOL							m_bAudio;
		BOOL							m_bVideo;
		FILE*							m_hFile;
		FLV_HEADER						m_header;
		vector<SPS>						m_pps;
		vector<PPS>						m_sps;
		TinyScopedPtr<AACDecode>		m_aac;
		TinyScopedPtr<H264Decode>		m_h264;
		TinyScopedPtr<MPG123Decode>		m_mpg123;
		FLV_SCRIPTDATA					m_script;
		AVCDecoderConfigurationRecord	m_avcconfig;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, LPVOID)>>	m_audioDone;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, LPVOID)>>	m_videoDone;
	};
}

