#pragma once
#include "Common/TinyCommon.h"
#include <dshow.h>
#include <strmif.h>
#include <uuids.h>
#include <string>
#include <list>
#include <vector>
#include "FilterBase.h"
#include "FilterObserver.h"
#include "FilterObserver.h"
#include "VideoCaptureParam.h"
#include "VideoCaptureCapability.h"
using namespace std;
using namespace TinyUI;

namespace Media
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
	extern GUID MediaSubTypeI420;
	extern GUID MediaSubTypeHDYC;
	/// <summary>
	/// 视频捕获设备
	/// </summary>
	class VideoCaptureDevice : public FilterObserver
	{
	public:
		class Name
		{
		public:
			Name();
			Name(const string& name, const string& id);
			~Name();
			const string& name() const;
			const string& id() const;
		private:
			string	m_name;
			string	m_id;
		};
	public:
		explicit VideoCaptureDevice();
		virtual ~VideoCaptureDevice();
		BOOL Initialize(const Name& name);
		virtual void Allocate(const VideoCaptureParam& params);
		virtual void DeAllocate();
	public:
		static BOOL GetDevices(vector<Name>& names);
		static BOOL GetDeviceParams(const VideoCaptureDevice::Name& device, vector<VideoCaptureParam>& formats);
		static BOOL GetDeviceFilter(const Name& name, IBaseFilter** filter);
	private:
		enum InternalState
		{
			Idle,//设备打开摄像头没使用
			Capturing, //视频采集中
			Error //出错
		};
		static BOOL GetPinCategory(IPin* pin, REFGUID category);
		static TinyComPtr<IPin> GetPin(IBaseFilter* filter, PIN_DIRECTION pin_dir, REFGUID category);
		static VideoPixelFormat TranslateMediaSubtypeToPixelFormat(const GUID& subType);
	private:
		BOOL CreateCapabilityMap();
	private:
		Name							m_currentName;
		VideoCaptureParam				m_currentParam;
		list<VideoCaptureCapability>	m_capabilitys;
		TinyComPtr<IBaseFilter>			m_captureFilter;
		TinyComPtr<IGraphBuilder>		m_graphBuilder;
		TinyComPtr<IMediaControl>		m_mediaControl;
		TinyComPtr<IPin>				m_capturePin;
		InternalState					m_state;
		VideoCaptureParam				m_vcf;
	};
}
