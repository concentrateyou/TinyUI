#pragma once
#include "FilterBase.h"
#include "FilterObserver.h"
#include "FilterObserver.h"
#include "VideoCaptureParam.h"
#include "VideoSinkFilter.h"
#include "ScopedMediaType.h"
using namespace std;
using namespace TinyFramework;

namespace DShow
{
	/// <summary>
	///  ”∆µ≤∂ªÒ
	/// </summary>
	class VideoCapture : public FilterObserver
	{
		DISALLOW_COPY_AND_ASSIGN(VideoCapture)
	public:
		class Name
		{
		public:
			Name();
			Name(string&& name, string&& id);
			Name(const string& name, const string& id);
			~Name();
			const string& name() const;
			const string& id() const;
		private:
			string	m_name;
			string	m_id;
		};
	public:
		VideoCapture();
		virtual ~VideoCapture();
		void	SetCallback(Callback<void(BYTE*, LONG, FLOAT, LPVOID)>&& callback);
		BOOL	Initialize(const Name& name);
		void	Uninitialize();
		BOOL	Start();
		BOOL	Stop();
		BOOL	Pause();
		BOOL	GetState(FILTER_STATE& state);
		BOOL	ShowProperty(HWND hWND);
	public:
		virtual BOOL Allocate(const VideoCaptureParam& param);
		virtual void Deallocate();
	public:
		void	OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, void*) OVERRIDE;
	public:
		static BOOL GetDevices(vector<Name>& names);
		static BOOL GetDeviceParams(const VideoCapture::Name& device, vector<VideoCaptureParam>& formats);
		static BOOL GetDeviceFilter(const Name& name, IBaseFilter** filter);
		static BOOL GetMediaType(IPin* pPin, REFGUID subtype, AM_MEDIA_TYPE** ppType);
	private:
		static VideoPixelFormat TranslateMediaSubtypeToPixelFormat(const GUID& subType);
		void SetAntiFlickerInCaptureFilter();
	private:
		TinyComPtr<IGraphBuilder>					m_builder;
		TinyComPtr<IMediaControl>					m_control;
		TinyComPtr<IBaseFilter>						m_captureFilter;
		TinyComPtr<IPin>							m_captureO;
		//MPEG
		TinyComPtr<IBaseFilter>						m_mjpgFilter;
		TinyComPtr<IPin>							m_mjpgO;//OUT
		TinyComPtr<IPin>							m_mjpgI;//IN
		//AV
		TinyComPtr<IBaseFilter>						m_avFilter;
		TinyComPtr<IPin>							m_avO;//OUT
		TinyComPtr<IPin>							m_avI;//IN
		TinyComPtr<IPin>							m_sinkI;
		TinyScopedReferencePtr<VideoSinkFilter>		m_sinkFilter;
		Callback<void(BYTE*, LONG, FLOAT, LPVOID)>	m_callback;
	};
}
