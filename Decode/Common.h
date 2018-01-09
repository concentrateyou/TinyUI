#pragma once
#include <stdio.h>
#include "Common/TinyCommon.h"
#include "Common/TinyCollection.h"
#include "Common/TinyCallback.h"
#include "Common/TinyEvent.h"
#include "Network/TinyHTTPClient.h"
#include "Render/TinyGDI.h"
#include "IO/TinyTask.h"
#include "IO/TinyIO.h"
#include "Media/TinyMedia.h"
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;

namespace Decode
{
#define TYPE_ONTEXTDATA 1
#define TYPE_ONCAPTION 2
#define TYPE_ONCAPTIONINFO 3
#define TYPE_UNKNOWN 9

#define TS_FEC_PACKET_SIZE 204
#define TS_DVHS_PACKET_SIZE 192
#define TS_PACKET_SIZE 188
#define TS_MAX_PACKET_SIZE 204

#define PES_START_SIZE  6
#define PES_HEADER_SIZE 9
#define MAX_PES_HEADER_SIZE (9 + 255)

	INT ToINT32(BYTE val[4]);
	INT ToINT24(BYTE val[3]);
	INT ToINT16(BYTE val[2]);
	INT ToINT8(BYTE val[1]);

	enum NALUType
	{
		NALU_TYPE_SLICE = 1,
		NALU_TYPE_DPA = 2,
		NALU_TYPE_DPB = 3,
		NALU_TYPE_DPC = 4,
		NALU_TYPE_IDR = 5,
		NALU_TYPE_SEI = 6,
		NALU_TYPE_SPS = 7,
		NALU_TYPE_PPS = 8,
		NALU_TYPE_AUD = 9,
		NALU_TYPE_EOSEQ = 10,
		NALU_TYPE_EOSTREAM = 11,
		NALU_TYPE_FILL = 12
	};
	enum NALUPriority
	{
		NALU_PRIORITY_DISPOSABLE = 0,
		NALU_PRIRITY_LOW = 1,
		NALU_PRIORITY_HIGH = 2,
		NALU_PRIORITY_HIGHEST = 3
	};
	//////////////////////////////////////////////////////////////////////////
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
		DOUBLE	audiochannels;
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
	//////////////////////////////////////////////////////////////////////////
	typedef struct tagTS_PACKEG_HEADER
	{
		BYTE Syncbyte;
		BYTE TransportErrorIndicator : 1;
		BYTE PayloadUnitStartIndicator : 1;
		BYTE TransportPriority : 1;
		SHORT PID : 13;
		BYTE TransportScramblingControl : 2;
		BYTE AdaptationFieldControl : 2;
		BYTE ContinuityCounter : 4;
	}TS_PACKEG_HEADER;

	typedef struct tagTS_PACKET_ADAPTATION_FIELD
	{
		BYTE AdaptationFieldLength : 8;
		BYTE DiscontinuityIndicator : 1;
		BYTE RandomAccessIndicator : 1;
		BYTE ElementaryStreamPriorityIndicator : 1;
		BYTE PCRFlag : 1;
		BYTE OPCRFlag : 1;
		BYTE SplicingPointFlag : 1;
		BYTE TransportPrivateDataFlag : 1;
		BYTE AdaptationFieldExtensionFlag : 1;
		UINT64 ProgramClockReferenceBase : 33;
		USHORT ProgramClockReferenceExtension : 9;
		UINT64 OriginalProgramClockReferenceBase : 33;
		USHORT OriginalProgramClockReferenceExtension : 9;
		BYTE SpliceCountdown;
		BYTE TransportPrivateDataLength : 8;
		BYTE AdaptationFieldExtensionLength : 8;
		BYTE ItwFlag : 1;
		BYTE PiecewiseRateFlag : 1;
		BYTE SeamlessSpliceFlag : 1;
		BYTE ItwValidFlag : 1;
		USHORT ItwOffset : 15;
		UINT32 PiecewiseRate : 22;
		BYTE SpliceType : 8;
		UINT64 DTSNextAU : 33;//三个部分
	}TS_PACKET_ADAPTATION_FIELD;

	typedef struct tagTS_PACKET_PROGRAM
	{
		USHORT ProgramNumber : 16;
		BYTE   Reserved : 3;
		USHORT ProgramPID : 13;
	}TS_PACKET_PROGRAM;

	typedef struct tagTS_PACKEG_PAT
	{
		BYTE TableID;
		BYTE SectionSyntaxIndicator : 1;
		BYTE Zero : 1;
		BYTE Reserved1 : 2;
		USHORT SectionLength : 12;
		USHORT TransportStreamID : 16;
		BYTE Reserved2 : 2;
		BYTE VersionNumber : 5;
		BYTE CurrentNextIndicator : 1;
		BYTE SectionNumber : 8;
		BYTE LastSectionNumber : 8;
		UINT32 CRC32;
	}TS_PACKEG_PAT;
}
