#pragma once
#include "DShowCommon.h"
#include "FilterObserver.h"
#include "AudioCaptureParam.h"
#include "AudioSinkFilter.h"
#include "Mixer.h"

namespace DShow
{
	class AudioCapture : public FilterObserver
	{
		DISALLOW_COPY_AND_ASSIGN(AudioCapture)
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
		void	OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) OVERRIDE;
	public:
		AudioCapture();
		virtual ~AudioCapture();
		BOOL	Initialize(const Name& name);
		BOOL	Initialize(const Name& name, Callback<void(BYTE*, LONG, FLOAT, LPVOID)>&& callback);
		void	Uninitialize();
		BOOL	Start();
		BOOL	Stop();
		BOOL	Pause();
		BOOL	GetState(FILTER_STATE& state);
		BOOL	SetVolume(LONG volume);
		BOOL	GetVolume(LONG& volume);
		BYTE*	GetPointer();
		LONG	GetSize();
		virtual BOOL Allocate(const AudioCaptureParam& param);
		virtual void DeAllocate();
	public:
		static BOOL GetDevices(vector<Name>& names);
		static BOOL GetDeviceParams(const AudioCapture::Name& device, vector<AudioCaptureParam>& formats);
		static BOOL GetDeviceFilter(const Name& name, IBaseFilter** filter);
	private:
		static BOOL GetPinCategory(IPin* pin, REFGUID category);
		static TinyComPtr<IPin> GetPin(IBaseFilter* filter, PIN_DIRECTION pin_dir, REFGUID category);
	private:
		LONG										m_size;
		IO::TinyRingQueue							m_queue;
		TinyScopedPtr<BYTE>							m_bits;
		TinyComPtr<IBaseFilter>						m_captureFilter;
		TinyComPtr<IGraphBuilder>					m_builder;
		TinyComPtr<IMediaControl>					m_control;
		TinyComPtr<IPin>							m_captureO;
		TinyComPtr<IPin>							m_sinkI;
		TinyComPtr<IAMAudioInputMixer>				m_mixer;
		TinyScopedReferencePtr<AudioSinkFilter>		m_sinkFilter;
		Callback<void(BYTE*, LONG, FLOAT, LPVOID)>	m_callback;
	};
}

