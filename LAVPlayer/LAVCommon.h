#pragma once
#include "DShowCommon.h"
#include "ScopedMediaType.h"
#include "FilterBase.h"
#include "FilterObserver.h"
#include "lav/ITrackInfo.h"
#include <dshow.h>
#include <dvdmedia.h>
#include <uuids.h>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;
using namespace TinyFramework;
using namespace DShow;
using namespace TinyFramework::Media;


namespace LAV
{
	BOOL WINAPI GetFilterByCLSID(const string& clsid, IBaseFilter** ps);

	class LAVAudioParam
	{
		DISALLOW_COPY_AND_ASSIGN(LAVAudioParam)
	};

	class LAVVideoParam
	{
		DISALLOW_COPY_AND_ASSIGN(LAVVideoParam)
	};

	class ScopedBSTR
	{
		DISALLOW_COPY_AND_ASSIGN(ScopedBSTR)
	public:
		ScopedBSTR(const CHAR* psText);
		~ScopedBSTR();
	private:
		BSTR	m_bstr;
	};
	/// <summary>
	///视频格式
	/// </summary>
	class  LAVVideoFormat
	{
	public:
		LAVVideoFormat();
		LAVVideoFormat(const TinySize& size, VideoPixelFormat vpf);
		BOOL IsValid() const;
		void SetFormat(VideoPixelFormat vpf);
		void SetSize(const TinySize& size);
		void SetSize(INT cx, INT cy);
		void Reset();
		const TinySize& GetSize() const;
		const VideoPixelFormat GetFormat() const;
		const string ToString() const;
		static string PixelFormatToString(VideoPixelFormat format);
	private:
		TinySize			m_size;
		VideoPixelFormat	m_vpf;
	};
	/// <summary>
	///音频格式
	/// </summary>
	class LAVAudioFormat
	{
	public:
		LAVAudioFormat();
		~LAVAudioFormat();
		WAVEFORMATEX GetFormat() const;
		void SetFormat(const WAVEFORMATEX& wfx);
		string ToString() const;
	private:
		WAVEFORMATEX	m_wft;
	};
}