#pragma once
#include "Render/TinyGDI.h"
#include "Media/TinyMedia.h"
#include <string>
using namespace std;
using namespace TinyFramework;
using namespace TinyFramework::Media;

namespace DShow
{
	class  VideoCaptureFormat
	{
	public:
		VideoCaptureFormat();
		VideoCaptureFormat(const TinySize& size, FLOAT rate, VideoPixelFormat vpf);
		BOOL IsValid() const;
		void SetFormat(VideoPixelFormat vpf);
		void SetSize(const TinySize& size);
		void SetSize(INT cx, INT cy);
		void SetScale(INT cx, INT cy);
		void SetRate(FLOAT rate);
		void Reset();
		const TinySize& GetSize() const;
		const TinySize&	GetScale() const;
		const FLOAT	GetRate() const;
		const VideoPixelFormat GetFormat() const;
		const string ToString() const;
		static string PixelFormatToString(VideoPixelFormat format);
	private:
		FLOAT				m_rate;
		TinySize			m_size;
		TinySize			m_scale;
		VideoPixelFormat	m_vpf;
	};

	class  VideoCaptureParam
	{
	public:
		VideoCaptureParam();
		~VideoCaptureParam();
		VideoCaptureFormat RequestFormat;
	};
}