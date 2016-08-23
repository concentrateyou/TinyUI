#pragma once
#include "VideoCommon.h"
#define __STDC_CONSTANT_MACROS  
extern "C"
{
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
};
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

namespace Media
{
	class BitmapConverter
	{
		DISALLOW_COPY_AND_ASSIGN(BitmapConverter);
	public:
		BitmapConverter(GUID subtype, INT cx, INT cy, INT scaleCX, INT scaleCY);
		~BitmapConverter();
		BOOL ToRGB24(const BYTE* sample, INT size, BYTE* pBits);
	private:
		INT				m_cx;
		INT				m_cy;
		INT				m_scaleCX;
		INT				m_scaleCY;
		AVFrame*		m_srcAV;
		AVFrame*		m_dstAV;
		SwsContext*		m_sws;
		AVPixelFormat	m_pixelFormat;
	};
	/*struct RGB24
	{
	BYTE*	data;
	INT		cx;
	INT		cy;
	};
	class BitmapConverter
	{
	public:
	BitmapConverter();
	~BitmapConverter();
	BOOL Initialize(GUID subtype, INT cx, INT cy, INT scaleCX, INT scaleCY);
	BOOL ToRGB24(const BYTE* ps, INT size, RGB24& rgb);
	AVPixelFormat GetPixelFormat();
	void Release();
	private:
	INT					m_cx;
	INT					m_cy;
	INT					m_scaleCX;
	INT					m_scaleCY;
	GUID				m_subtype;
	AVFrame*			m_src_av;
	AVFrame*			m_dst_av;
	SwsContext*			m_sws;
	};*/
	/*class BitmapConverter
	{
	public:
	enum StreamFormat
	{
	BGR32,
	RGB32,
	RGB24,
	BGR24,
	PAL8,
	YUV420P,
	YUV422,
	YUV444
	};

	BitmapConverter(
	StreamFormat src_format,
	uint32_t src_width,
	uint32_t src_height,
	StreamFormat dst_format,
	uint32_t dst_width,
	uint32_t dst_height,
	bool reverse = false,
	bool geometric = false,
	bool auto_fill = false,
	int32_t line_size = 0);

	~BitmapConverter();

	void convert(const uint8_t *src_buffer, uint8_t *dst_buffer);


	private:
	BitmapConverter(const BitmapConverter &) {}
	BitmapConverter &operator=(const BitmapConverter &) {}

	void get_centered_video_position(uint32_t src_width, uint32_t src_height, uint32_t dst_width, uint32_t dst_height,
	uint32_t &picture_x, uint32_t &picture_y, uint32_t &picture_width, uint32_t &picture_height);

	void get_filled_video_position(
	uint32_t src_width,
	uint32_t src_height,
	uint32_t dst_width,
	uint32_t dst_height,
	uint32_t &picture_x,
	uint32_t &picture_y,
	uint32_t &picture_width,
	uint32_t &picture_height);

	SwsContext *        sws_context_;
	bool                reverse_;
	uint32_t            src_width_;
	uint32_t            src_height_;
	uint32_t            dst_width_;
	uint32_t            dst_height_;
	uint32_t            picture_x_;
	uint32_t            picture_y_;
	uint32_t            picture_width_;
	uint32_t            picture_height_;
	uint8_t *           back_buffer_;
	uint8_t *           back_fill_buffer_;
	PixelFormat         src_pixel_;
	PixelFormat         dst_pixel_;
	AVFrame *           src_frame_;
	AVFrame *           dst_frame_;
	uint8_t *           temp_src_;
	int32_t             line_size_;
	};*/
}


