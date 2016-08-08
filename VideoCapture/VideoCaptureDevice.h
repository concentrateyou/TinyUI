#pragma once
#include "Common/TinyCommon.h"
#include "FilterBase.h"
#include "FilterObserver.h"
#include "VideoCaptureFormat.h"
#include <dshow.h>
#include <strmif.h>
#include <uuids.h>
#include <string>
using namespace TinyUI;
using namespace std;

namespace VideoCapture
{
	class ScopedMediaType
	{
	public:
		ScopedMediaType();
		~ScopedMediaType();
		AM_MEDIA_TYPE*	operator->();
		AM_MEDIA_TYPE*	Ptr();
		AM_MEDIA_TYPE** Receive();
	private:
		void Release();
		AM_MEDIA_TYPE* m_mediaType;
	};
	/// <summary>
	/// 视频捕获设备
	/// </summary>
	class VideoCaptureDevice : public FilterObserver
	{
	public:
		explicit VideoCaptureDevice(const string& name);
		virtual ~VideoCaptureDevice();
		BOOL Initialize();
	private:
		enum InternalState
		{
			Idle,//设备打开摄像头没使用
			Capturing, //视频采集中
			Error //出错
		};
		static BOOL GetDeviceFilter(const string& name, IBaseFilter** filter);
		static BOOL PinMatchesCategory(IPin* pin, REFGUID category);
		static TinyComPtr<IPin> GetPin(IBaseFilter* filter, PIN_DIRECTION pin_dir, REFGUID category);
		static VideoPixelFormat TranslateMediaSubtypeToPixelFormat(const GUID& sub_type);
	private:
		TinyComPtr<IBaseFilter>		m_captureFilter;
		TinyComPtr<IGraphBuilder>	m_graph;
		InternalState				m_state;
		VideoCaptureFormat			m_vcf;
	private:

	};
}
