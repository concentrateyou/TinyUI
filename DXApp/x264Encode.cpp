#include "stdafx.h"
#include "x264Encode.h"


x264Encode::x264Encode()
	:m_x264Image(NULL),
	m_hx264(NULL),
	m_x264Param(NULL)
{
}

BOOL x264Encode::Open(INT cx, INT cy)
{

	if (!BuildParam(cx, cy))
		return FALSE;
	if ((m_hx264 = x264_encoder_open(m_x264Param)) == NULL)
		return FALSE;
	if ((m_x264Image = new x264_picture_t()) == NULL)
		return FALSE;
	x264_picture_init(m_x264Image);
	m_x264Image->img.i_csp = X264_CSP_I420;
	m_x264Image->img.i_plane = 3;
	return TRUE;
}

BOOL x264Encode::BuildParam(INT cx, INT cy)
{

	m_bits.Reset(new BYTE[1024 * 1024 * 4]);

	m_x264Param = new x264_param_t();
	if (!m_x264Param)
		return FALSE;
	x264_param_default(m_x264Param);
	x264_param_default_preset(m_x264Param, "superfast", "zerolatency");
	m_x264Param->i_frame_total = 0;
	m_x264Param->i_threads = 0;
	m_x264Param->b_sliced_threads = 0;
	m_x264Param->i_csp = X264_CSP_I420;
	m_x264Param->i_keyint_max = 90;
	m_x264Param->i_width = cx;
	m_x264Param->i_height = cy;
	m_x264Param->i_fps_num = 30;
	m_x264Param->i_fps_den = 1;
	m_x264Param->b_cabac = 1; /*cabac的开关*/
	m_x264Param->rc.b_mb_tree = 0;
	m_x264Param->rc.i_rc_method = X264_RC_ABR;
	m_x264Param->rc.f_rf_constant = 25;
	m_x264Param->rc.f_rf_constant_max = 45;
	m_x264Param->rc.i_bitrate = 1024;/*设置平均码率大小*/
	m_x264Param->rc.f_rate_tolerance = 0.1F;
	m_x264Param->rc.i_vbv_max_bitrate = static_cast<INT>(1024 * 1.2);
	return TRUE;
}

BOOL x264Encode::Encode(AVFrame* pI420, RTMPPublisher* publisher)
{
	m_size = 0;
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
	m_x264Image->i_pts = (LONGLONG)m_x264Param->i_frame_total;
	x264_picture_t image;
	x264_nal_t * nal = NULL;
	INT i_nal = 0;
	INT size = x264_encoder_encode(m_hx264, &nal, &i_nal, m_x264Image, &image);
	for (INT i = 0; i < i_nal; i++)
	{
		if (nal[i].i_type == NAL_SPS)
		{
			publisher->SetSPS(nal[i].p_payload + 4, nal[i].i_payload - 4);

		}
		else if (nal[i].i_type == NAL_PPS)
		{
			publisher->SetPPS(nal[i].p_payload + 4, nal[i].i_payload - 4);
			publisher->SendVideoSPSPPS();

		}
		else
		{
			publisher->SendVideo(nal[i].p_payload, size - m_size);
		}
		m_size += nal[i].i_payload;
	}
	return TRUE;
}

void x264Encode::Close()
{
	if (m_hx264)
	{
		x264_encoder_close(m_hx264);
		m_hx264 = NULL;
	}
	SAFE_DELETE(m_x264Param);
	SAFE_DELETE(m_x264Image);
}

BYTE* x264Encode::GetPointer() const
{
	return m_bits;
}
INT	x264Encode::GetSize() const
{
	return m_size;
}

x264Encode::~x264Encode()
{
}
