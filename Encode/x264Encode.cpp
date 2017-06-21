#include "stdafx.h"
#include "x264Encode.h"

namespace Encode
{
	x264Encode::x264Encode()
		:m_x264Image(NULL),
		m_x264(NULL),
		m_x264Param(NULL),
		m_sINC(0)
	{
	}

	x264Encode::~x264Encode()
	{
		Close();
	}

	BOOL x264Encode::Open(INT cx, INT cy, INT fps, INT rate)
	{
		Close();
		if (!BuildParam(cx, cy, fps, rate))
			return FALSE;
		if ((m_x264 = x264_encoder_open(m_x264Param)) == NULL)
			return FALSE;
		if ((m_x264Image = new x264_picture_t()) == NULL)
			return FALSE;
		x264_picture_init(m_x264Image);
		m_x264Image->img.i_csp = X264_CSP_I420;
		m_x264Image->img.i_plane = 3;
		m_x264Image->i_pts = 0;
		return TRUE;
	}

	BOOL x264Encode::BuildParam(INT cx, INT cy, INT fps, INT bitrate)
	{
		m_x264Param = new x264_param_t();
		if (!m_x264Param)
			return FALSE;
		x264_param_default(m_x264Param);
		x264_param_default_preset(m_x264Param, "superfast", "zerolatency");
		m_x264Param->i_frame_total = 0;
		m_x264Param->i_threads = 0;
		m_x264Param->b_sliced_threads = 0;
		m_x264Param->b_repeat_headers = 1;//重复SPS/PPS 放到关键帧前面 
		m_x264Param->i_csp = X264_CSP_I420;
		m_x264Param->i_width = cx;
		m_x264Param->i_height = cy;
		m_x264Param->i_fps_num = fps;
		m_x264Param->i_fps_den = 1;
		m_x264Param->i_keyint_max = m_x264Param->i_fps_num * 2;
		m_x264Param->b_cabac = 1; /*cabac的开关*/
		m_x264Param->i_bframe = 3;
		m_x264Param->i_bframe_adaptive = X264_B_ADAPT_FAST;
		m_x264Param->rc.b_mb_tree = 0;
		m_x264Param->rc.i_rc_method = X264_RC_ABR;//CQP(恒定质量)，CRF(恒定码率)，ABR(平均码率)
		m_x264Param->rc.f_rf_constant = 25;
		m_x264Param->rc.f_rf_constant_max = 40;
		m_x264Param->rc.i_bitrate = bitrate;/*设置平均码率大小*/
		m_x264Param->rc.f_rate_tolerance = 0.1F;
		m_x264Param->rc.i_vbv_max_bitrate = static_cast<INT>(bitrate * 1.2);
		m_sPTS = (1000 / m_x264Param->i_fps_num);
		x264_param_apply_profile(m_x264Param, "baseline");
		return TRUE;
	}

	BOOL x264Encode::Encode(AVFrame* pI420)
	{
		if (!m_x264Image || !pI420)
			return FALSE;
		m_x264Image->img.plane[0] = pI420->data[0];
		m_x264Image->img.plane[1] = pI420->data[1];
		m_x264Image->img.plane[2] = pI420->data[2];
		m_x264Image->img.plane[3] = pI420->data[3];
		m_x264Image->img.i_stride[0] = pI420->linesize[0];
		m_x264Image->img.i_stride[1] = pI420->linesize[1];
		m_x264Image->img.i_stride[2] = pI420->linesize[2];
		m_x264Image->img.i_stride[3] = pI420->linesize[3];
		m_x264Image->i_type = X264_TYPE_AUTO;
		x264_picture_t image;
		x264_nal_t * pNAL = NULL;
		INT iNAL = 0;
		INT size = x264_encoder_encode(m_x264, &pNAL, &iNAL, m_x264Image, &image);
		if (size && iNAL)
		{
			m_x264Image->i_pts++;
			for (INT i = 0; i < iNAL; i++)
			{
				switch (pNAL[i].i_type)
				{
				case NAL_SPS:
				{
					BYTE* sps = pNAL[i].p_payload + SPS_SEP;
					INT	size = pNAL[i].i_payload - SPS_SEP;
					MediaTag tag = { 0 };
					tag.dwType = 0;
					tag.PTS = m_sPTS;
					tag.DTS = image.i_dts;
					tag.dwTime = timeGetTime();
					tag.dwFlag = pNAL[i].i_type;
					tag.INC = ++m_sINC;
					//OnDone(sps, size, tag);
				}
				break;
				case NAL_PPS:
				{
					BYTE* pps = pNAL[i].p_payload + PPS_SEP;
					INT size = pNAL[i].i_payload - PPS_SEP;
					MediaTag tag = { 0 };
					tag.dwType = 0;
					tag.PTS = m_sPTS;
					tag.DTS = image.i_dts;
					tag.dwTime = timeGetTime();
					tag.dwFlag = pNAL[i].i_type;
					tag.INC = ++m_sINC;
					//OnDone(pps, size, tag);
				}
				break;
				case NAL_SLICE:
				case NAL_SLICE_DPA:
				case NAL_SLICE_DPB:
				case NAL_SLICE_DPC:
				case NAL_SLICE_IDR:
				{
					BYTE* bits = pNAL[i].p_payload;
					INT	size = pNAL[i].i_payload;
					MediaTag tag = { 0 };
					tag.dwType = 0;
					tag.PTS = m_sPTS;
					tag.DTS = image.i_dts;
					tag.dwTime = timeGetTime();
					tag.dwFlag = pNAL[i].i_type;
					tag.INC = ++m_sINC;
					//OnDone(bits, size, tag);
				}
				break;
				}
			}
			return TRUE;
		}
		return FALSE;
	}
	void x264Encode::Close()
	{
		if (m_x264)
		{
			x264_encoder_close(m_x264);
			m_x264 = NULL;
		}
		SAFE_DELETE(m_x264Param);
		SAFE_DELETE(m_x264Image);
	}
}