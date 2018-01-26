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
#include "IO/TinyIOBuffer.h"
#include "IO/TinyBitReader.h"
#include "IO/TinyBitWriter.h"
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

#define ADTS_HEADER_MIN_SIZE	7

#define TS_STREAM_TYPE_VIDEO_MPEG1     0x01  
#define TS_STREAM_TYPE_VIDEO_MPEG2     0x02  
#define TS_STREAM_TYPE_AUDIO_MPEG1     0x03  
#define TS_STREAM_TYPE_AUDIO_MPEG2     0x04  
#define TS_STREAM_TYPE_PRIVATE_SECTION 0x05  
#define TS_STREAM_TYPE_PRIVATE_DATA    0x06  
#define TS_STREAM_TYPE_AUDIO_AAC       0x0f  
#define TS_STREAM_TYPE_VIDEO_MPEG4     0x10  
#define TS_STREAM_TYPE_VIDEO_H264      0x1b  
#define TS_STREAM_TYPE_AUDIO_AC3       0x81  
#define TS_STREAM_TYPE_AUDIO_DTS       0x8a 

	INT ToINT32(BYTE val[4]);
	INT ToINT24(BYTE val[3]);
	INT ToINT16(BYTE val[2]);
	INT ToINT8(BYTE val[1]);

	BOOL FindStartCode(BYTE* bits, INT size);

	typedef struct tagH264NALU
	{
		const BYTE*	bits;
		LONG		size;
		BYTE		type;//NALU类型
		BYTE		sliceType;//帧类型
	}H264NALU;

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

	enum SliceType
	{
		P = 0,
		B = 1,
		I = 2,
		SP = 3,
		SI = 4
	};

	enum NALUPriority
	{
		NALU_PRIORITY_DISPOSABLE = 0,
		NALU_PRIRITY_LOW = 1,
		NALU_PRIORITY_HIGH = 2,
		NALU_PRIORITY_HIGHEST = 3
	};

	enum VideoCodec
	{
		UnknownVideoCodec = 0,
		CodecH264,
		CodecVC1,
		CodecMPEG2,
		CodecMPEG4,
		CodecTheora,
		CodecVP8,
		CodecVP9,
		CodecHEVC,
		VideoCodecMax = CodecHEVC
	};

	enum AudioCodec
	{
		UnknownAudioCodec = 0,
		CodecAAC = 1,
		CodecMP3 = 2,
		CodecPCM = 3,
		CodecVorbis = 4,
		CodecFLAC = 5,
		CodecAMR_NB = 6,
		CodecAMR_WB = 7,
		CodecPCM_MULAW = 8,
		CodecGSM_MS = 9,
		CodecPCM_S16BE = 10,
		CodecPCM_S24BE = 11,
		CodecOpus = 12,
		CodecEAC3 = 13,
		CodecPCM_ALAW = 14,
		CodecALAC = 15,
		CodecAC3 = 16,
		kAudioCodecMax = CodecAC3,
	};

	namespace H264
	{
		enum H264ProfileIDC
		{
			ProfileIDCBaseline = 66,
			ProfileIDCConstrainedBaseline = ProfileIDCBaseline,
			ProfileIDCMain = 77,
			ProfileIDScalableBaseline = 83,
			ProfileIDScalableHigh = 86,
			ProfileIDCHigh = 100,
			ProfileIDHigh10 = 110,
			ProfileIDSMultiviewHigh = 118,
			ProfileIDHigh422 = 122,
			ProfileIDStereoHigh = 128,
			ProfileIDHigh444Predictive = 244,
		};
	}


	enum  VideoCodecProfile
	{
		VIDEO_CODEC_PROFILE_UNKNOWN = -1,
		VIDEO_CODEC_PROFILE_MIN = VIDEO_CODEC_PROFILE_UNKNOWN,
		H264PROFILE_MIN = 0,
		H264PROFILE_BASELINE = H264PROFILE_MIN,
		H264PROFILE_MAIN = 1,
		H264PROFILE_EXTENDED = 2,
		H264PROFILE_HIGH = 3,
		H264PROFILE_HIGH10PROFILE = 4,
		H264PROFILE_HIGH422PROFILE = 5,
		H264PROFILE_HIGH444PREDICTIVEPROFILE = 6,
		H264PROFILE_SCALABLEBASELINE = 7,
		H264PROFILE_SCALABLEHIGH = 8,
		H264PROFILE_STEREOHIGH = 9,
		H264PROFILE_MULTIVIEWHIGH = 10,
		H264PROFILE_MAX = H264PROFILE_MULTIVIEWHIGH,
		VP8PROFILE_MIN = 11,
		VP8PROFILE_ANY = VP8PROFILE_MIN,
		VP8PROFILE_MAX = VP8PROFILE_ANY,
		VP9PROFILE_MIN = 12,
		VP9PROFILE_PROFILE0 = VP9PROFILE_MIN,
		VP9PROFILE_PROFILE1 = 13,
		VP9PROFILE_PROFILE2 = 14,
		VP9PROFILE_PROFILE3 = 15,
		VP9PROFILE_MAX = VP9PROFILE_PROFILE3,
		HEVCPROFILE_MIN = 16,
		HEVCPROFILE_MAIN = HEVCPROFILE_MIN,
		HEVCPROFILE_MAIN10 = 17,
		HEVCPROFILE_MAIN_STILL_PICTURE = 18,
		HEVCPROFILE_MAX = HEVCPROFILE_MAIN_STILL_PICTURE,
		VIDEO_CODEC_PROFILE_MAX = HEVCPROFILE_MAX,
	};

	enum  VideoPixelFormat
	{
		PIXEL_FORMAT_UNKNOWN = 0,  // Unknown or unspecified format value.
		PIXEL_FORMAT_I420 = 1,  // 12bpp YUV planar 1x1 Y, 2x2 UV samples, a.k.a. YU12.
		PIXEL_FORMAT_YV12 = 2,   // 12bpp YVU planar 1x1 Y, 2x2 VU samples.
		PIXEL_FORMAT_YV16 = 3,   // 16bpp YVU planar 1x1 Y, 2x1 VU samples.
		PIXEL_FORMAT_YV12A = 4,  // 20bpp YUVA planar 1x1 Y, 2x2 VU, 1x1 A samples.
		PIXEL_FORMAT_YV24 = 5,   // 24bpp YUV planar, no subsampling.
		PIXEL_FORMAT_NV12 = 6,  // 12bpp with Y plane followed by a 2x2 interleaved UV plane.
		PIXEL_FORMAT_NV21 = 7,  // 12bpp with Y plane followed by a 2x2 interleaved VU plane.
		PIXEL_FORMAT_UYVY = 8,  // 16bpp interleaved 2x1 U, 1x1 Y, 2x1 V, 1x1 Y samples.
		PIXEL_FORMAT_YUY2 = 9,  // 16bpp interleaved 1x1 Y, 2x1 U, 1x1 Y, 2x1 V samples.
		PIXEL_FORMAT_ARGB = 10,   // 32bpp ARGB, 1 plane.
		PIXEL_FORMAT_XRGB = 11,   // 24bpp XRGB, 1 plane.
		PIXEL_FORMAT_RGB24 = 12,  // 24bpp BGR, 1 plane.
		PIXEL_FORMAT_RGB32 = 13,  // 32bpp BGRA, 1 plane.
		PIXEL_FORMAT_MJPEG = 14,  // MJPEG compressed.
		PIXEL_FORMAT_MT21 = 15,
		PIXEL_FORMAT_YUV420P9 = 16,
		PIXEL_FORMAT_YUV420P10 = 17,
		PIXEL_FORMAT_YUV422P9 = 18,
		PIXEL_FORMAT_YUV422P10 = 19,
		PIXEL_FORMAT_YUV444P9 = 20,
		PIXEL_FORMAT_YUV444P10 = 21,
		PIXEL_FORMAT_MAX = PIXEL_FORMAT_YUV444P10,  // Must always be equal to largest entry logged.
	};

	typedef struct tagH264PPS
	{
		INT32 pic_parameter_set_id;
		INT32 seq_parameter_set_id;
		BOOL entropy_coding_mode_flag;
		BOOL bottom_field_pic_order_in_frame_present_flag;
		INT32 num_slice_groups_minus1;
		INT32 num_ref_idx_l0_default_active_minus1;
		INT32 num_ref_idx_l1_default_active_minus1;
		BOOL weighted_pred_flag;
		INT32 weighted_bipred_idc;
		INT32 pic_init_qp_minus26;
		INT32 pic_init_qs_minus26;
		INT32 chroma_qp_index_offset;
		BOOL deblocking_filter_control_present_flag;
		BOOL constrained_intra_pred_flag;
		BOOL redundant_pic_cnt_present_flag;
		BOOL transform_8x8_mode_flag;
		BOOL pic_scaling_matrix_present_flag;
		INT32 scaling_list4x4[6][16];
		INT32 scaling_list8x8[6][64];
		INT32 second_chroma_qp_index_offset;
	}H264PPS;

	typedef struct tagH264SPS
	{
		INT32 profile_idc;
		INT32 constraint_set0_flag;
		INT32 constraint_set1_flag;
		INT32 constraint_set2_flag;
		INT32 constraint_set3_flag;
		INT32 constraint_set4_flag;
		INT32 constraint_set5_flag;
		INT32 level_idc;
		INT32 seq_parameter_set_id;
		INT32 chroma_format_idc;
		BOOL separate_colour_plane_flag;
		INT32 bit_depth_luma_minus8;
		INT32 bit_depth_chroma_minus8;
		BOOL qpprime_y_zero_transform_bypass_flag;
		BOOL seq_scaling_matrix_present_flag;
		BOOL seq_scaling_list_present_flag;
		INT32 scaling_list4x4[6][16];
		INT32 scaling_list8x8[6][64];
		INT32 log2_max_frame_num_minus4;
		INT32 pic_order_cnt_type;
		INT32 log2_max_pic_order_cnt_lsb_minus4;
		BOOL delta_pic_order_always_zero_flag;
		INT32 offset_for_non_ref_pic;
		INT32 offset_for_top_to_bottom_field;
		INT32 num_ref_frames_in_pic_order_cnt_cycle;
		INT32 expected_delta_per_pic_order_cnt_cycle;
		INT32 offset_for_ref_frame[255];
		INT32 max_num_ref_frames;
		BOOL gaps_in_frame_num_value_allowed_flag;
		INT32 pic_width_in_mbs_minus1;
		INT32 pic_height_in_map_units_minus1;
		BOOL frame_mbs_only_flag;	//frame_mbs_only_flag等于0指明了视频序列的编码图象可能是编码场或编码帧 
									//frame_mbs_only_flag等于1指明了每个编码视频序列的编码图像都是只含帧宏块的编码帧
		BOOL mb_adaptive_frame_field_flag;
		BOOL direct_8x8_inference_flag;
		BOOL frame_cropping_flag;
		INT32 frame_crop_left_offset;
		INT32 frame_crop_right_offset;
		INT32 frame_crop_top_offset;
		INT32 frame_crop_bottom_offset;
		BOOL vui_parameters_present_flag;
		INT32 sar_width;
		INT32 sar_height;
		BOOL bitstream_restriction_flag;
		INT32 max_num_reorder_frames;
		INT32 max_dec_frame_buffering;
		BOOL nal_hrd_parameters_present_flag;
		INT32 cpb_cnt_minus1;
		INT32 bit_rate_scale;
		INT32 cpb_size_scale;
		INT32 bit_rate_value_minus1[32];
		INT32 cpb_size_value_minus1[32];
		BOOL cbr_flag[32];
		INT32 initial_cpb_removal_delay_length_minus_1;
		INT32 cpb_removal_delay_length_minus1;
		INT32 dpb_output_delay_length_minus1;
		INT32 time_offset_length;
		BOOL low_delay_hrd_flag;
		INT32 chroma_array_type;
		BOOL aspect_ratio_info_present_flag;
		INT32 aspect_ratio_idc;
		BOOL overscan_info_present_flag;
		BOOL overscan_appropriate_flag;
		BOOL video_signal_type_present_flag;
		INT32 video_format;
		BOOL video_full_range_flag;
		BOOL colour_description_present_flag;
		BOOL chroma_loc_info_present_flag;
		INT32 chroma_sample_loc_type_top_field;
		INT32 chroma_sample_loc_type_bottom_field;
		BOOL timing_info_present_flag;
		INT32 num_units_in_tick;
		INT32 time_scale;
		BOOL fixed_frame_rate_flag;
	}H264SPS;

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
	//PSI(Program Specific Information)
	//	–PAT(Program Association Table)节目关联表
	//	–PMT(Program Map Table)节目映射表
	//	–CAT(Conditional Access Table)条件接收表
	//SI(Service Information)
	//	–NIT(Network Information Table)网络信息表
	//	–SDT(Service Discription Table)业务描述信息表
	//	–BAT(Bouquet Association Table)业务群信息表
	//	–EIT(Event Information Table)节目事件信息表
	//	–TDT(Time and Data Table) 日期时间表
	typedef struct tagTS_PACKEG_HEADER
	{
		BYTE Syncbyte;
		BYTE TransportErrorIndicator : 1;
		BYTE PayloadUnitStartIndicator : 1;//PSI和PES处理
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

	class TS_PACKET_PROGRAM
	{
	public:
		USHORT ProgramNumber : 16;
		BYTE   Reserved : 3;
		USHORT ProgramPID : 13;
	public:
		BOOL operator == (const TS_PACKET_PROGRAM& other);
	};

	class TS_PACKET_SECTION
	{
	public:
		BYTE TableID;
		BYTE SectionSyntaxIndicator : 1;
		BYTE Zero : 1;
		BYTE Reserved1 : 2;
		USHORT SectionLength : 12;
	};

	class TS_PACKET_PAT : public TS_PACKET_SECTION
	{
	public:
		USHORT TransportStreamID : 16;
		BYTE Reserved2 : 2;
		BYTE VersionNumber : 5;
		BYTE CurrentNextIndicator : 1;
		BYTE SectionNumber : 8;
		BYTE LastSectionNumber : 8;
		UINT32 CRC32;
	};

	class TS_PACKET_PMT : public TS_PACKET_SECTION
	{
	public:
		USHORT ProgramNumber : 16;
		BYTE Reserved2 : 2;
		BYTE VersionNumber : 5;
		BYTE CurrentNextIndicator : 1;
		BYTE SectionNumber : 8;
		BYTE LastSectionNumber : 8;
		BYTE Reserved3 : 3;
		USHORT PCR_PID : 13;
		BYTE Reserved4 : 4;
		USHORT ProgramInfoLength : 12;
		UINT32 CRC32;
	};

	typedef struct tagTS_PACKET_PES
	{
		UINT32 PacketStartCodePrefix : 24;
		BYTE StreamID;
		USHORT PESPacketLength;
		BYTE PESScramblingControl : 2;
		BYTE PESPriority : 1;
		BYTE DataAlignmentIndicator : 1;
		BYTE Copyright : 1;
		BYTE OriginalOrCopy : 1;
		BYTE PTSDTSFlags : 2;
		BYTE ESCRFlag : 1;
		BYTE ESRateFlag : 1;
		BYTE DSMTrickModeFlag : 1;
		BYTE AdditionalCopyInfoFlag : 1;
		BYTE PESCRCFlag : 1;
		BYTE PESExtensionFlag : 1;
		BYTE PESHeaderDataLength;
		BYTE TrickModeControl : 3;
		BYTE FieldID : 2;
		BYTE IntraSliceRefresh : 1;
		BYTE FrequencyTruncation : 2;
		BYTE RepCntrl : 5;
		BYTE AdditionalCopyInfo : 7;
		USHORT PreviousPESPacketCRC;
		BYTE PESPrivateDataFlag : 1;
		BYTE PackHeaderFieldFlag : 1;
		BYTE ProgramPacketSequenceCounterFlag : 1;
		BYTE PSTDBufferFlag : 1;
		BYTE PESExtensionFlag2 : 1;
		BYTE PackFieldLength : 8;
		BYTE ProgramPacketSequenceCounter : 7;
		BYTE MPEG1MPEG2Identifier : 1;
		BYTE OriginalStuffLength : 6;
		BYTE PSTDBufferScale : 1;
		USHORT PSTDBufferSize : 13;
		BYTE PESExtensionFieldLength : 7;
		BYTE StreamIDExtensionFlag : 1;
		BYTE StreamIDExtension : 7;
		UINT64 PTS;
		UINT64 DTS;
		UINT64 ESCR;
		UINT32 ESRate;
	}TS_PACKET_PES;

	typedef struct tagTS_BLOCK
	{
		BYTE		streamType;
		LONGLONG	dts;
		LONGLONG	pts;
		union
		{
			struct
			{
				BYTE*	data;
				LONG	size;
				BYTE	type;
			}audio;
			struct
			{
				BYTE*	data;
				LONG	size;
				BYTE	type;
				BYTE	codeType;
			}video;
		};
	}TS_BLOCK;

	class TS_PACKET_DESCRIPTOR
	{
	public:
		BYTE DescriptorTag;
		BYTE DescriptorLength;
		CHAR Context[256];
	};

	typedef Callback<void(const BYTE*, LONG, LPVOID)> ConfigCallback;

	class H264VideoConfig
	{
	public:
		H264VideoConfig();
		H264VideoConfig(VideoCodec codec,
			VideoCodecProfile profile,
			VideoPixelFormat format,
			INT frameRate,
			const TinySize& codedSize,
			const TinyRectangle& visibleRect,
			const TinySize& naturalSize);
		H264VideoConfig(const H264VideoConfig& o);
		BOOL operator == (const H264VideoConfig& o);
		BOOL operator != (const H264VideoConfig& o);
	private:
		VideoPixelFormat	m_pixelFormat;
		VideoCodec			m_codec;
		VideoCodecProfile	m_profile;
		INT					m_frameRate;
		TinySize			m_codedSize;
		TinySize			m_naturalSize;
		TinyRectangle		m_visibleRect;
	};

	class AACAudioConfig
	{
	public:
		AACAudioConfig();
		AACAudioConfig(AudioCodec codec, const WAVEFORMAT& waveFMT);
		AACAudioConfig(const AACAudioConfig& o);
		BOOL operator == (const AACAudioConfig& o);
		BOOL operator != (const AACAudioConfig& o);
	private:
		AudioCodec		m_codec;
		WAVEFORMAT		m_waveFMT;
	};
}
