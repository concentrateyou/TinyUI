#pragma once
#include "MFCommon.h"

namespace MF
{
	class  MFAudioCaptureParam
	{
	public:
		MFAudioCaptureParam();
		MFAudioCaptureParam(const TinySize& size, FLOAT rate, VideoPixelFormat vpf, DWORD dwIndex);
		BOOL IsValid() const;
		static VideoPixelFormat GetFormat(const GUID& guid);
		void SetFormat(VideoPixelFormat vpf);
		void SetSize(const TinySize& size);
		void SetSize(INT cx, INT cy);
		void SetScale(INT cx, INT cy);
		void SetRate(FLOAT rate);
		void SetStreamIndex(DWORD dwIndex);
		const TinySize& GetSize() const;
		const TinySize&	GetScale() const;
		const DWORD GetStreamIndex() const;
		FLOAT	GetRate() const;
		VideoPixelFormat GetFormat() const;
		string ToString() const;
		static string PixelFormatToString(VideoPixelFormat format);
	private:
		DWORD				m_dwIndex;
		FLOAT				m_rate;
		TinySize			m_size;
		TinySize			m_scale;
		VideoPixelFormat	m_vpf;
	};
}
