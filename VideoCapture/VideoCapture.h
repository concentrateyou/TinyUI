#pragma once
#include "FilterBase.h"
#include "FilterObserver.h"
#include "FilterObserver.h"
#include "VideoCaptureParam.h"
#include "SinkFilter.h"
#include "ScopedMediaType.h"
using namespace std;
using namespace TinyUI;

namespace Media
{
	/// <summary>
	///  ”∆µ≤∂ªÒ
	/// </summary>
	class VideoCapture : public FilterObserver
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
		void OnFrameReceive(const BYTE* data, INT size, void*) OVERRIDE;
	public:
		explicit VideoCapture();
		virtual ~VideoCapture();
		BOOL Initialize(const Name& name);
		void Uninitialize();
		BOOL Start();
		BOOL Stop();
		BOOL Pause();
		FILTER_STATE GetState() const;
		virtual BOOL Allocate(const VideoCaptureParam& param);
		virtual void DeAllocate();
	public:
		static BOOL GetDevices(vector<Name>& names);
		static BOOL GetDeviceParams(const VideoCapture::Name& device, vector<VideoCaptureParam>& formats);
		static BOOL GetDeviceFilter(const Name& name, IBaseFilter** filter);
	private:
		static BOOL GetPinCategory(IPin* pin, REFGUID category);
		static TinyComPtr<IPin> GetPin(IBaseFilter* filter, PIN_DIRECTION pin_dir, REFGUID category);
		static VideoPixelFormat TranslateMediaSubtypeToPixelFormat(const GUID& subType);
		void SetAntiFlickerInCaptureFilter();
	private:
		VideoCaptureParam					m_vcf;
		TinyComPtr<IGraphBuilder>			m_builder;
		TinyComPtr<IMediaControl>			m_control;
		TinyComPtr<IBaseFilter>				m_captureFilter;
		TinyComPtr<IPin>					m_captureConnector;
		TinyComPtr<IBaseFilter>				m_mjpgFilter;
		TinyComPtr<IPin>					m_mjpgConnector1;
		TinyComPtr<IPin>					m_mjpgConnector2;
		TinyComPtr<IPin>					m_sinkConnector;
		TinyScopedReferencePtr<SinkFilter>	m_sinkFilter;
	};
}
