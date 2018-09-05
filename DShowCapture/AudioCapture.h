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

	public:
		AudioCapture();
		virtual ~AudioCapture();
		BOOL	IsEmpty() const;
		void	SetCallback(Callback<void(BYTE*, LONG, REFERENCE_TIME, LPVOID)>&& callback);
		BOOL	Initialize(const Name& name);
		void	Uninitialize();
		BOOL	Start();
		BOOL	Stop();
		BOOL	Pause();
		BOOL	GetState(FILTER_STATE& state);
		BOOL	SetVolume(LONG volume);
		BOOL	GetVolume(LONG& volume);
	public:
		void	OnFrameReceive(BYTE* bits, LONG size, REFERENCE_TIME timestamp, LPVOID lpParameter) OVERRIDE;
	public:
		virtual BOOL Allocate(const AudioCaptureParam& param);
		virtual void Deallocate();
	public:
		static BOOL GetDevices(vector<Name>& names);
		static BOOL GetDeviceFormats(const AudioCapture::Name& device, vector<AudioCaptureFormat>& formats);
		static BOOL GetDeviceFilter(const Name& name, IBaseFilter** filter);
	private:
		TinyComPtr<IBaseFilter>								m_captureFilter;
		TinyComPtr<IGraphBuilder>							m_builder;
		TinyComPtr<IMediaControl>							m_control;
		TinyComPtr<IPin>									m_captureO;
		TinyComPtr<IPin>									m_sinkI;
		TinyComPtr<IAMAudioInputMixer>						m_mixer;
		TinyScopedReferencePtr<AudioSinkFilter>				m_sinkFilter;
		Callback<void(BYTE*, LONG, REFERENCE_TIME, LPVOID)>	m_callback;
	};
}

