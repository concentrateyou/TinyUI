#include "stdafx.h"
#include "x264Encode.h"


x264Encode::x264Encode()
	:m_x264Image(NULL),
	m_hx264(NULL),
	m_x264Param(NULL),
	m_pBits(NULL)
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
	m_x264Param = new x264_param_t();
	if (!m_x264Param)
		return FALSE;
	ZeroMemory(&m_x264Param, sizeof(x264_param_t));
	x264_param_default(m_x264Param);
	x264_param_default_preset(m_x264Param, "superfast", "zerolatency");
	m_x264Param->i_frame_total = 0;
	m_x264Param->i_threads = 0;
	m_x264Param->b_sliced_threads = 0;
	m_x264Param->i_csp = X264_CSP_I420;
	m_x264Param->i_width = cx;
	m_x264Param->i_height = cy;
	m_x264Param->i_fps_num = 30;
	m_x264Param->i_fps_den = 1;
	m_x264Param->b_cabac = 1; /*cabac的开关*/
	m_x264Param->rc.i_rc_method = X264_RC_ABR;/*恒定码率*/
	m_x264Param->rc.i_bitrate = 1000;/*设置平均码率大小*/
	m_x264Param->rc.f_rate_tolerance = 0.1F;
	m_x264Param->rc.i_vbv_max_bitrate = static_cast<INT>(1000 * 1.2);
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
	m_x264Image->i_pts = (LONGLONG)m_x264Param->i_frame_total;
	x264_picture_t image;
	x264_nal_t * nal = NULL;
	INT i_nal = 0;
	INT encode_error_code = x264_encoder_encode(m_hx264, &nal, &i_nal, m_x264Image, &image);
	UINT size = 0;
	for (INT i = 0; i < i_nal; i++)
	{
		if (memcmp(nal[i].p_payload, "\0\0\1", 3) == 0)
		{
			memcpy(m_pBits + size, "\0", 1);
			size++;
		}
		memcpy(m_pBits + size, nal[i].p_payload, nal[i].i_payload);
		size += nal[i].i_payload;
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

x264Encode::~x264Encode()
{
}
