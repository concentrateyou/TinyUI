#pragma once
#include "DShowCommon.h"
#include "FilterObserver.h"
#include "AudioCaptureParam.h"
#include "AudioSinkFilter.h"

namespace Media
{
	class AudioCapture : public FilterObserver
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
		AudioCapture();
		virtual ~AudioCapture();
		BOOL Initialize(const Name& name, Callback<void(const BYTE*, INT, LPVOID)>& callback);
		void Uninitialize();
		BOOL Start();
		BOOL Stop();
		BOOL Pause();
		BOOL GetState(FILTER_STATE& state);
		virtual BOOL Allocate(const AudioCaptureParam& param);
		virtual void DeAllocate();
		void OnFrameReceive(const BYTE* data, INT size, LPVOID lpParameter) OVERRIDE;
		void SetVolume(INT volume);
		INT GetVolume() const;
	public:
		static BOOL GetDevices(vector<Name>& names);
		static BOOL GetDeviceParams(const AudioCapture::Name& device, vector<AudioCaptureParam>& formats);
		static BOOL GetDeviceFilter(const Name& name, IBaseFilter** filter);
	private:
		static BOOL GetPinCategory(IPin* pin, REFGUID category);
		static TinyComPtr<IPin> GetPin(IBaseFilter* filter, PIN_DIRECTION pin_dir, REFGUID category);
	private:
		TinyComPtr<IBaseFilter>						m_captureFilter;
		TinyComPtr<IGraphBuilder>					m_builder;
		TinyComPtr<IMediaControl>					m_control;
		TinyComPtr<IPin>							m_captureO;
		TinyComPtr<IPin>							m_sinkI;
		TinyScopedReferencePtr<AudioSinkFilter>		m_sinkFilter;
		Callback<void(const BYTE*, INT, LPVOID)>	m_callback;
	};
}

