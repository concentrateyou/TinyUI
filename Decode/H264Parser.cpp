#include "stdafx.h"
#include "H264Parser.h"
#include <limits>
#include <algorithm>
using namespace std;

namespace Decode
{
	static const INT Default4x4Intra[16] =
	{
		6, 13, 13, 20, 20, 20, 28, 28, 28, 28, 32, 32, 32, 37, 37, 42
	};

	static const INT Default4x4Inter[16] =
	{
		10, 14, 14, 20, 20, 20, 24, 24, 24, 24, 27, 27, 27, 30, 30, 34
	};

	static const INT Default8x8Intra[64] =
	{
		6, 10, 10, 13, 11, 13, 16, 16, 16, 16, 18, 18, 18, 18, 18, 23,
		23, 23, 23, 23, 23, 25, 25, 25, 25, 25, 25, 25, 27, 27, 27, 27,
		27, 27, 27, 27, 29, 29, 29, 29, 29, 29, 29, 31, 31, 31, 31, 31,
		31, 33, 33, 33, 33, 33, 36, 36, 36, 36, 38, 38, 38, 40, 40, 42
	};

	static const INT Default8x8Inter[64] =
	{
		9, 13, 13, 15, 13, 15, 17, 17, 17, 17, 19, 19, 19, 19, 19, 21,
		21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 24, 24, 24, 24,
		24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 27, 27, 27, 27, 27,
		27, 28, 28, 28, 28, 28, 30, 30, 30, 30, 32, 32, 32, 33, 33, 35,
	};

	static const INT TableSarWidth[] =
	{
		0, 1, 12, 10, 16, 40, 24, 20, 32, 80, 18, 15, 64, 160, 4, 3, 2
	};

	static const INT TableSarHeight[] =
	{
		0, 1, 11, 11, 11, 33, 11, 11, 11, 33, 11, 11, 33, 99, 3, 2, 1
	};

	static inline void DefaultScalingList4x4(INT i, INT scaling_list4x4[][16])
	{
		if (i < 3)
			memcpy(scaling_list4x4[i], Default4x4Intra, sizeof(Default4x4Intra));
		else if (i < 6)
			memcpy(scaling_list4x4[i], Default4x4Inter, sizeof(Default4x4Inter));
	}

	static inline void DefaultScalingList8x8(INT i, INT scaling_list8x8[][64])
	{
		if (i % 2 == 0)
			memcpy(scaling_list8x8[i], Default8x8Intra, sizeof(Default8x8Intra));
		else
			memcpy(scaling_list8x8[i], Default8x8Intra, sizeof(Default8x8Inter));
	}

	static void FallbackScalingList4x4(INT i, const INT default_scaling_list_intra[], const INT default_scaling_list_inter[], INT scaling_list4x4[][16])
	{
		static const INT ScalingList4x4ByteSize = sizeof(scaling_list4x4[0][0]) * 16;
		switch (i)
		{
		case 0:
			memcpy(scaling_list4x4[i], default_scaling_list_intra, ScalingList4x4ByteSize);
			break;
		case 1:
			memcpy(scaling_list4x4[i], scaling_list4x4[0], ScalingList4x4ByteSize);
			break;
		case 2:
			memcpy(scaling_list4x4[i], scaling_list4x4[1], ScalingList4x4ByteSize);
			break;
		case 3:
			memcpy(scaling_list4x4[i], default_scaling_list_inter, ScalingList4x4ByteSize);
			break;
		case 4:
			memcpy(scaling_list4x4[i], scaling_list4x4[3], ScalingList4x4ByteSize);
			break;
		case 5:
			memcpy(scaling_list4x4[i], scaling_list4x4[4], ScalingList4x4ByteSize);
			break;
		default:
			break;
		}
	}

	static void FallbackScalingList8x8(INT i, const INT default_scaling_list_intra[], const INT default_scaling_list_inter[], INT scaling_list8x8[][64])
	{
		static const INT ScalingList8x8ByteSize = sizeof(scaling_list8x8[0][0]) * 64;
		switch (i)
		{
		case 0:
			memcpy(scaling_list8x8[i], default_scaling_list_intra, ScalingList8x8ByteSize);
			break;
		case 1:
			memcpy(scaling_list8x8[i], default_scaling_list_inter, ScalingList8x8ByteSize);
			break;
		case 2:
			memcpy(scaling_list8x8[i], scaling_list8x8[0], ScalingList8x8ByteSize);
			break;
		case 3:
			memcpy(scaling_list8x8[i], scaling_list8x8[1], ScalingList8x8ByteSize);
			break;
		case 4:
			memcpy(scaling_list8x8[i], scaling_list8x8[2], ScalingList8x8ByteSize);
			break;
		case 5:
			memcpy(scaling_list8x8[i], scaling_list8x8[3], ScalingList8x8ByteSize);
			break;
		default:
			break;
		}
	}

	static void FillDefaultSeqScalingLists(H264SPS& sps)
	{
		for (INT i = 0; i < 6; ++i)
		{
			for (INT j = 0; j < 16; ++j)
			{
				sps.scaling_list4x4[i][j] = 16;
			}
		}

		for (INT i = 0; i < 6; ++i)
		{
			for (INT j = 0; j < 64; ++j)
			{
				sps.scaling_list8x8[i][j] = 16;
			}
		}
	}

	VideoCodecProfile ProfileIDC2VideoCodecProfile(INT profileIDC)
	{
		switch (profileIDC)
		{
		case H264::ProfileIDCBaseline:
			return H264PROFILE_BASELINE;
		case H264::ProfileIDCMain:
			return H264PROFILE_MAIN;
		case H264::ProfileIDCHigh:
			return H264PROFILE_HIGH;
		case H264::ProfileIDHigh10:
			return H264PROFILE_HIGH10PROFILE;
		case H264::ProfileIDHigh422:
			return H264PROFILE_HIGH422PROFILE;
		case H264::ProfileIDHigh444Predictive:
			return H264PROFILE_HIGH444PREDICTIVEPROFILE;
		case H264::ProfileIDScalableBaseline:
			return H264PROFILE_SCALABLEBASELINE;
		case H264::ProfileIDScalableHigh:
			return H264PROFILE_SCALABLEHIGH;
		case H264::ProfileIDStereoHigh:
			return H264PROFILE_STEREOHIGH;
		case H264::ProfileIDSMultiviewHigh:
			return H264PROFILE_MULTIVIEWHIGH;
		}
		return VIDEO_CODEC_PROFILE_UNKNOWN;
	}
	//////////////////////////////////////////////////////////////////////////
	H264Parser::H264Parser(ConfigCallback& callback)
		:m_callback(callback)
	{

	}
	H264Parser::~H264Parser()
	{

	}
	const H264VideoConfig& H264Parser::GetVideoConfig() const
	{
		return m_lastConfig;
	}
	BOOL H264Parser::FindCode(const BYTE* bits, LONG size, LONG& offset, BYTE& code)
	{
		if (size <= 0)
			return FALSE;
		LONG remainingBytes = size;
		while (remainingBytes >= 3)
		{
			if (bits[0] == 0x00 && bits[1] == 0x00 && bits[2] == 0x01)
			{
				offset = size - remainingBytes;
				code = 3;
				if (offset > 0 && *(bits - 1) == 0x00)
				{
					--offset;
					++code;
				}
				return TRUE;
			}
			++bits;
			--remainingBytes;
		}
		offset = size;
		code = 0;
		return TRUE;
	}
	BOOL H264Parser::Parse(const BYTE* bits, LONG size, H264NALU& sNALU)
	{
		if (!bits || size <= 0)
			return FALSE;
		BYTE mask = 0;
		ZeroMemory(&sNALU, sizeof(sNALU));
		H264NALU s;
		ZeroMemory(&s, sizeof(s));
		BYTE code = 0;
		for (;;)
		{
			LONG offset = 0;
			if (!FindCode(bits, size, offset, code))
				break;
			if (offset > 0)
			{
				s.size = offset;
				switch (s.type)
				{
				case NALU_TYPE_AUD://分隔符
					break;
				case NALU_TYPE_SPS:
				{
					mask = code;
					INT spsID = 0;
					m_sps.resize(s.size + 4);
					memcpy_s(&m_sps[0], s.size + 4, s.bits - 4, s.size + 4);
					if (!ParseSPS(s.bits + 1, s.size - 1, code, spsID))
					{
						m_sps.clear();
						return FALSE;
					}
					H264SPS* sps = m_spsMap.GetValue(spsID);
					if (!sps)
						return FALSE;
					SetVideoConfig(sps, code);
				}
				break;
				case NALU_TYPE_PPS:
				{
					mask = code;
					INT ppsID = 0;
					m_pps.resize(s.size + code);
					memcpy_s(&m_pps[0], s.size + code, s.bits - code, s.size + code);
					if (!ParsePPS(s.bits + 1, s.size - 1, code, ppsID))
					{
						m_pps.clear();
						return FALSE;
					}
					H264PPS* pps = m_ppsMap.GetValue(ppsID);
					if (!pps)
						return FALSE;
					H264SPS* sps = m_spsMap.GetValue(pps->seq_parameter_set_id);
					if (!sps)
						return FALSE;
					SetVideoConfig(sps, code);
				}
				break;
				case NALU_TYPE_SLICE:
				case NALU_TYPE_IDR:
				{
					sNALU.type = s.type;
					sNALU.bits = s.bits - 4;//起始码
					sNALU.size = s.size + 4;
					if (!ParseSliceHeader(s.bits + 1, s.size - 1, code, sNALU))
					{
						return FALSE;
					}
				}
				break;
				}
			}
			bits += (offset + code);
			size -= (offset + code);
			s.type = bits[0] & 0x1F;
			s.bits = bits;
		}
		return TRUE;
	}
	BOOL H264Parser::ParseSPS(const BYTE* bits, LONG size, BYTE code, INT& spsID)
	{
		m_reader.Initialize(bits, size);
		INT value = 0;
		H264SPS sps = { 0 };
		if (!m_reader.ReadBits(8, &sps.profile_idc))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set0_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set1_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set2_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set3_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set4_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set5_flag))
			return FALSE;
		if (!m_reader.ReadBits(2, &value))
			return FALSE;
		if (!m_reader.ReadBits(8, &sps.level_idc))
			return FALSE;
		if (!ReadUE(&sps.seq_parameter_set_id))
			return FALSE;
		if (sps.seq_parameter_set_id >= 32)
			return FALSE;
		if (sps.profile_idc == 100 || sps.profile_idc == 110 ||
			sps.profile_idc == 122 || sps.profile_idc == 244 ||
			sps.profile_idc == 44 || sps.profile_idc == 83 ||
			sps.profile_idc == 86 || sps.profile_idc == 118 ||
			sps.profile_idc == 128)
		{
			if (!ReadUE(&sps.chroma_format_idc))
				return FALSE;
			if (sps.chroma_format_idc >= 4)
				return FALSE;
			if (sps.chroma_format_idc == 3)
			{
				if (!m_reader.ReadBits(1, &sps.separate_colour_plane_flag))
					return FALSE;
			}
			if (!ReadUE(&sps.bit_depth_luma_minus8))
				return FALSE;
			if (sps.bit_depth_luma_minus8 >= 7)
				return FALSE;
			if (!ReadUE(&sps.bit_depth_chroma_minus8))
				return FALSE;
			if (sps.bit_depth_chroma_minus8 >= 7)
				return FALSE;
			if (!m_reader.ReadBits(1, &sps.qpprime_y_zero_transform_bypass_flag))
				return FALSE;
			if (!m_reader.ReadBits(1, &sps.seq_scaling_matrix_present_flag))
				return FALSE;
			if (sps.seq_scaling_matrix_present_flag)
			{
				if (!ParseSPSScalingLists(sps))
					return FALSE;
			}
			else
			{
				FillDefaultSeqScalingLists(sps);
			}
		}
		else
		{
			sps.chroma_format_idc = 1;
			FillDefaultSeqScalingLists(sps);
		}
		if (sps.separate_colour_plane_flag)
		{
			sps.chroma_array_type = 0;
		}
		else
		{
			sps.chroma_array_type = sps.chroma_format_idc;
		}
		if (!ReadUE(&sps.log2_max_frame_num_minus4))
			return FALSE;
		if (sps.log2_max_frame_num_minus4 >= 13)
			return FALSE;
		if (!ReadUE(&sps.pic_order_cnt_type))
			return FALSE;
		if (sps.pic_order_cnt_type >= 3)
			return FALSE;
		sps.expected_delta_per_pic_order_cnt_cycle = 0;
		if (sps.pic_order_cnt_type == 0)
		{
			if (!ReadUE(&sps.log2_max_pic_order_cnt_lsb_minus4))
				return FALSE;
			if (sps.log2_max_pic_order_cnt_lsb_minus4 >= 13)
				return FALSE;
		}
		else if (sps.pic_order_cnt_type == 1)
		{
			if (!m_reader.ReadBits(1, &sps.delta_pic_order_always_zero_flag))
				return FALSE;
			if (!ReadSE(&sps.offset_for_non_ref_pic))
				return FALSE;
			if (!ReadSE(&sps.offset_for_top_to_bottom_field))
				return FALSE;
			if (!ReadUE(&sps.num_ref_frames_in_pic_order_cnt_cycle))
				return FALSE;
			if (sps.num_ref_frames_in_pic_order_cnt_cycle >= 255)
				return FALSE;
			for (INT i = 0; i < sps.num_ref_frames_in_pic_order_cnt_cycle; ++i)
			{
				if (!ReadSE(&sps.offset_for_ref_frame[i]))
					return FALSE;
				sps.expected_delta_per_pic_order_cnt_cycle += sps.offset_for_ref_frame[i];
			}
		}
		if (!ReadUE(&sps.max_num_ref_frames))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.gaps_in_frame_num_value_allowed_flag))
			return FALSE;
		if (!ReadUE(&sps.pic_width_in_mbs_minus1))
			return FALSE;
		if (!ReadUE(&sps.pic_height_in_map_units_minus1))
			return FALSE;
		//frame_mbs_only_flag等于0指明了视频序列的编码图象可能是编码场或编码帧
		//frame_mbs_only_flag等于1指明了每个编码视频序列的编码图像都是只含帧宏块的编码帧。
		if (!m_reader.ReadBits(1, &sps.frame_mbs_only_flag))
			return FALSE;
		if (!sps.frame_mbs_only_flag)
		{
			//mb_adaptive_frame_field_flag等于0表明在一个图像内不能切换使用帧和场宏块
			//mb_adaptive_frame_field_flag等于1表示在一帧中有可能使用场和帧的切换,当mb_adaptive_frame_field_flag没有设定的时候，应该赋给0.
			if (!m_reader.ReadBits(1, &sps.mb_adaptive_frame_field_flag))
				return FALSE;
		}
		if (!m_reader.ReadBits(1, &sps.direct_8x8_inference_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.frame_cropping_flag))
			return FALSE;
		if (sps.frame_cropping_flag)
		{
			if (!ReadUE(&sps.frame_crop_left_offset))
				return FALSE;
			if (!ReadUE(&sps.frame_crop_right_offset))
				return FALSE;
			if (!ReadUE(&sps.frame_crop_top_offset))
				return FALSE;
			if (!ReadUE(&sps.frame_crop_bottom_offset))
				return FALSE;
		}
		if (!m_reader.ReadBits(1, &sps.vui_parameters_present_flag))
			return FALSE;
		if (sps.vui_parameters_present_flag)
		{
			if (!ParseVUIParameters(sps))
				return FALSE;
		}
		spsID = sps.seq_parameter_set_id;
		m_spsMap.SetAt(sps.seq_parameter_set_id, sps);
		return TRUE;
	}
	BOOL H264Parser::ParsePPS(const BYTE* bits, LONG size, BYTE code, INT& ppsID)
	{
		m_reader.Initialize(bits, size);
		INT value = 0;
		H264PPS pps = { 0 };
		if (!ReadUE(&pps.pic_parameter_set_id))
			return FALSE;
		if (!ReadUE(&pps.seq_parameter_set_id))
			return FALSE;
		if (pps.seq_parameter_set_id >= 32)
			return FALSE;
		H264SPS* sps = m_spsMap.GetValue(pps.seq_parameter_set_id);
		if (!sps)
			return FALSE;
		if (!m_reader.ReadBits(1, &pps.entropy_coding_mode_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &pps.bottom_field_pic_order_in_frame_present_flag))
			return FALSE;
		if (!ReadUE(&pps.num_slice_groups_minus1))
			return FALSE;
		if (pps.num_slice_groups_minus1 > 1)
			return FALSE;
		if (!ReadUE(&pps.num_ref_idx_l0_default_active_minus1))
			return FALSE;
		if (pps.num_slice_groups_minus1 >= 32)
			return FALSE;
		if (!ReadUE(&pps.num_ref_idx_l1_default_active_minus1))
			return FALSE;
		if (pps.num_ref_idx_l1_default_active_minus1 >= 32)
			return FALSE;
		if (!m_reader.ReadBits(1, &pps.weighted_pred_flag))
			return FALSE;
		if (!m_reader.ReadBits(2, &pps.weighted_bipred_idc))
			return FALSE;
		if (pps.weighted_bipred_idc >= 3)
			return FALSE;
		if (!ReadSE(&pps.pic_init_qp_minus26))
			return FALSE;
		if (pps.pic_init_qp_minus26 < -26 || pps.pic_init_qp_minus26 > 25)
			return FALSE;
		if (!ReadSE(&pps.pic_init_qs_minus26))
			return FALSE;
		if (pps.pic_init_qs_minus26 < -26 || pps.pic_init_qs_minus26 > 25)
			return FALSE;
		if (!ReadSE(&pps.chroma_qp_index_offset))
			return FALSE;
		if (pps.chroma_qp_index_offset < -12 || pps.chroma_qp_index_offset > 12)
			return FALSE;
		pps.second_chroma_qp_index_offset = pps.chroma_qp_index_offset;
		if (!m_reader.ReadBits(1, &pps.deblocking_filter_control_present_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &pps.constrained_intra_pred_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &pps.redundant_pic_cnt_present_flag))
			return FALSE;
		if (m_reader.HasRBSP())
		{
			if (!m_reader.ReadBits(1, &pps.transform_8x8_mode_flag))
				return FALSE;
			if (!m_reader.ReadBits(1, &pps.pic_scaling_matrix_present_flag))
				return FALSE;
			if (pps.pic_scaling_matrix_present_flag)
			{
				if (!ParsePPSScalingLists(*sps, pps))
					return FALSE;
			}
			if (!ReadSE(&pps.second_chroma_qp_index_offset))
				return FALSE;
		}
		ppsID = pps.pic_parameter_set_id;
		m_ppsMap.SetAt(pps.pic_parameter_set_id, pps);
		return TRUE;
	}
	BOOL H264Parser::ParseSliceHeader(const BYTE* bits, LONG size, BYTE code, H264NALU& s)
	{
		m_reader.Initialize(bits, size);
		INT first_mb_in_slice = 0;
		if (!ReadUE(&first_mb_in_slice))
			return FALSE;
		INT slice_type = 0;
		if (!ReadUE(&slice_type))
			return FALSE;
		s.sliceType = static_cast<BYTE>(slice_type);
		return TRUE;
	}
	BOOL H264Parser::ReadUE(INT* val)
	{
		INT count = -1;
		INT value = 0;
		do
		{
			if (!m_reader.ReadBits(1, &value))
				return FALSE;
			count++;
		} while (value == 0);
		if (count > 31)
			return FALSE;
		*val = (1u << count) - 1u;
		if (count == 31)
		{
			if (!m_reader.ReadBits(count, &value))
				return FALSE;
			return (value == 0);
		}
		if (count > 0)
		{
			if (!m_reader.ReadBits(count, &value))
				return FALSE;
			*val += value;
		}
		return TRUE;
	}
	BOOL H264Parser::ReadSE(INT* val)
	{
		INT iUE = 0;
		if (!ReadUE(&iUE))
			return FALSE;
		if (iUE % 2 == 0)
			*val = -(iUE / 2);
		else
			*val = iUE / 2 + 1;
		return TRUE;
	}
	BOOL H264Parser::SetVideoConfig(const H264SPS* sps, BYTE code)
	{
		ASSERT(sps);
		INT sarcx = (sps->sar_width == 0) ? 1 : sps->sar_width;
		INT sarcy = (sps->sar_height == 0) ? 1 : sps->sar_height;
		TinySize codedSize(16 * (sps->pic_width_in_mbs_minus1 + 1), 16 * (sps->pic_height_in_map_units_minus1 + 1));
		if (codedSize.cx > (std::numeric_limits<LONG>::max)() / 1 || codedSize.cy > (std::numeric_limits<LONG>::max)() / 1)
			return FALSE;
		TinyRectangle visibleRect(sps->frame_crop_left_offset,
			sps->frame_crop_top_offset,
			(codedSize.cx - sps->frame_crop_right_offset) - sps->frame_crop_left_offset,
			(codedSize.cy - sps->frame_crop_bottom_offset) -
			sps->frame_crop_top_offset);
		if (visibleRect.Width() <= 0 || visibleRect.Height() <= 0)
			return FALSE;
		if (visibleRect.Width() > (std::numeric_limits<INT>::max)() / sarcx)
			return FALSE;
		TinySize naturalSize((visibleRect.Width() * sarcx) / sarcy, visibleRect.Height());
		if (naturalSize.cx == 0)
			return FALSE;
		INT frameRate = sps->num_units_in_tick > 0 ? sps->time_scale / (sps->num_units_in_tick * 2) : 0;
		H264VideoConfig config(CodecH264, ProfileIDC2VideoCodecProfile(sps->profile_idc), PIXEL_FORMAT_YV12, frameRate, codedSize, visibleRect, naturalSize);
		if (m_lastConfig != config)
		{
			//默认支持单个SPS和PPS
			if (m_sps.size() > 0 && m_pps.size() > 0)
			{
				m_lastConfig = config;
				if (!m_callback.IsNull())
				{
					TinyBufferArray<BYTE> buffer;
					buffer.Add(&m_sps[0], m_sps.size());
					buffer.Add(&m_pps[0], m_pps.size());
					m_callback(buffer.GetPointer(), buffer.GetSize(), TS_STREAM_TYPE_VIDEO_H264, this);
				}
			}
		}
		return TRUE;
	}
	BOOL H264Parser::ParseSPSScalingLists(H264SPS& sps)
	{
		BOOL bDefault;
		for (INT i = 0; i < 6; ++i)
		{
			if (m_reader.ReadBits(1, &sps.seq_scaling_list_present_flag))
				return FALSE;
			if (sps.seq_scaling_list_present_flag)
			{
				if (!ParseScalingList(arraysize(sps.scaling_list4x4[i]), sps.scaling_list4x4[i], &bDefault))
					return FALSE;
				if (bDefault)
				{
					DefaultScalingList4x4(i, sps.scaling_list4x4);
				}

			}
			else
			{
				FallbackScalingList4x4(i, Default4x4Intra, Default4x4Inter, sps.scaling_list4x4);
			}
		}
		for (INT i = 0; i < ((sps.chroma_format_idc != 3) ? 2 : 6); ++i)
		{
			if (m_reader.ReadBits(1, &sps.seq_scaling_list_present_flag))
				return FALSE;
			if (sps.seq_scaling_list_present_flag)
			{
				if (!ParseScalingList(arraysize(sps.scaling_list8x8[i]), sps.scaling_list8x8[i], &bDefault))
					return FALSE;
				if (bDefault)
				{
					DefaultScalingList8x8(i, sps.scaling_list8x8);
				}
			}
			else
			{
				FallbackScalingList8x8(i, Default8x8Intra, Default8x8Inter, sps.scaling_list8x8);
			}
		}
		return TRUE;
	}
	BOOL H264Parser::ParseScalingList(INT size, INT* scaling_list, BOOL* bDefault)
	{
		INT lastScale = 8;
		INT nextScale = 8;
		INT deltaScale;
		*bDefault = FALSE;
		for (INT i = 0; i < size; ++i)
		{
			if (nextScale != 0)
			{
				if (!ReadUE(&deltaScale))
					return FALSE;
				if (deltaScale < -128 || deltaScale >127)
					return FALSE;
				nextScale = (lastScale + deltaScale + 256) & 0xFF;
				if (i == 0 && nextScale == 0)
				{
					*bDefault = TRUE;
					return TRUE;
				}
			}
			scaling_list[i] = (nextScale == 0) ? lastScale : nextScale;
			lastScale = scaling_list[i];
		}
		return TRUE;
	}
	BOOL H264Parser::ParseVUIParameters(H264SPS& sps)
	{
		if (!m_reader.ReadBits(1, &sps.aspect_ratio_info_present_flag))
			return FALSE;
		if (sps.aspect_ratio_info_present_flag)
		{
			if (!m_reader.ReadBits(8, &sps.aspect_ratio_idc))
				return FALSE;
			if (sps.aspect_ratio_idc == 255)
			{
				if (!m_reader.ReadBits(16, &sps.sar_width))
					return FALSE;
				if (!m_reader.ReadBits(16, &sps.sar_height))
					return FALSE;
			}
			else
			{
				const INT max_aspect_ratio_idc = arraysize(TableSarWidth) - 1;
				if (sps.aspect_ratio_idc < 0 || sps.aspect_ratio_idc > max_aspect_ratio_idc)
					return FALSE;
				sps.sar_width = TableSarWidth[sps.aspect_ratio_idc];
				sps.sar_height = TableSarHeight[sps.aspect_ratio_idc];
			}
		}
		if (!m_reader.ReadBits(1, &sps.overscan_info_present_flag))
			return FALSE;
		if (sps.overscan_info_present_flag)
		{
			if (!m_reader.ReadBits(1, &sps.overscan_appropriate_flag))
				return FALSE;
		}
		if (!m_reader.ReadBits(1, &sps.video_signal_type_present_flag))
			return FALSE;
		if (sps.video_signal_type_present_flag)
		{
			if (!m_reader.ReadBits(3, &sps.video_format))
				return FALSE;
			if (!m_reader.ReadBits(1, &sps.video_full_range_flag))
				return FALSE;
			if (!m_reader.ReadBits(1, &sps.colour_description_present_flag))
				return FALSE;
			if (sps.colour_description_present_flag)
			{
				INT32 val = 0;
				if (!m_reader.ReadBits(24, &val))//忽略
					return FALSE;
			}
		}
		if (!m_reader.ReadBits(1, &sps.chroma_loc_info_present_flag))
			return FALSE;
		if (sps.chroma_loc_info_present_flag)
		{
			if (!ReadUE(&sps.chroma_sample_loc_type_top_field))
				return FALSE;
			if (!ReadUE(&sps.chroma_sample_loc_type_bottom_field))
				return FALSE;
		}
		if (!m_reader.ReadBits(1, &sps.timing_info_present_flag))
			return FALSE;
		if (sps.timing_info_present_flag)
		{
			INT32 val1 = 0;
			INT32 val2 = 0;
			if (!m_reader.ReadBits(16, &val1))
				return FALSE;
			if (!m_reader.ReadBits(16, &val2))
				return FALSE;
			sps.num_units_in_tick = val1 << 16 | val2;
			if (!m_reader.ReadBits(16, &val1))
				return FALSE;
			if (!m_reader.ReadBits(16, &val2))
				return FALSE;
			sps.time_scale = val1 << 16 | val2;
			if (!m_reader.ReadBits(1, &sps.fixed_frame_rate_flag))
				return FALSE;
		}
		return TRUE;
	}
	BOOL H264Parser::ParsePPSScalingLists(const H264SPS& sps, H264PPS& pps)
	{
		BOOL pic_scaling_list_present_flag;
		BOOL bDefault;
		for (INT i = 0; i < 6; ++i)
		{
			if (!m_reader.ReadBits(1, &pic_scaling_list_present_flag))
				return FALSE;
			if (pic_scaling_list_present_flag)
			{
				if (!ParseScalingList(arraysize(pps.scaling_list4x4[i]), pps.scaling_list4x4[i], &bDefault))
					return FALSE;
				if (bDefault)
				{
					DefaultScalingList4x4(i, pps.scaling_list4x4);
				}

			}
			else
			{
				if (sps.seq_scaling_matrix_present_flag)
				{
					FallbackScalingList4x4(i, Default4x4Intra, Default4x4Inter, pps.scaling_list4x4);
				}
				else
				{
					FallbackScalingList4x4(i, sps.scaling_list4x4[0], sps.scaling_list4x4[3], pps.scaling_list4x4);
				}
			}
		}

		if (pps.transform_8x8_mode_flag)
		{
			for (INT i = 0; i < ((sps.chroma_format_idc != 3) ? 2 : 6); ++i)
			{
				if (!m_reader.ReadBits(1, &pic_scaling_list_present_flag))
					return FALSE;
				if (pic_scaling_list_present_flag)
				{
					if (!ParseScalingList(arraysize(pps.scaling_list8x8[i]), pps.scaling_list8x8[i], &bDefault))
						return FALSE;
					if (bDefault)
					{
						DefaultScalingList8x8(i, pps.scaling_list8x8);
					}

				}
				else
				{
					if (sps.seq_scaling_matrix_present_flag)
					{
						FallbackScalingList8x8(i, Default8x8Intra, Default8x8Inter, pps.scaling_list8x8);
					}
					else
					{
						FallbackScalingList8x8(i, sps.scaling_list8x8[0], sps.scaling_list8x8[1], pps.scaling_list8x8);
					}
				}
			}
		}
		return TRUE;
	}
}

