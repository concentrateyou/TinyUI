#pragma once
#include "MFCommon.h"
#include "MFObserver.h"
#include "MFReaderCallback.h"
#include "MFAudioCaptureParam.h"

namespace MF
{
	class MFAudioCapture : public MFObserver
	{
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
		MFAudioCapture();
		virtual ~MFAudioCapture();
		BOOL	Initialize(const Name& name);
		BOOL	Initialize(const Name& name, Callback<void(BYTE*, LONG, FLOAT, LPVOID)>&& receiveCB);
		void	Uninitialize();
		void	OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) OVERRIDE;
	public:
		virtual BOOL Allocate(const MFAudioCaptureParam& param);
		virtual void Deallocate();
	public:
		static BOOL GetFormat(const GUID& guid, VideoPixelFormat* format);
		static BOOL GetDevices(vector<MFAudioCapture::Name>& names);
		static BOOL GetDeviceParams(const MFAudioCapture::Name& device, vector<MFAudioCaptureParam>& params);
		static BOOL GetDeviceSource(const MFAudioCapture::Name& device, IMFMediaSource** source);
	protected:
		BOOL											m_bCapturing;
		TinyComPtr<IMFSourceReader>						m_reader;
		TinyComPtr<IMFMediaSource>						m_source;
		TinyScopedReferencePtr<MFAudioReaderCallback>	m_readerCB;
		Callback<void(BYTE*, LONG, FLOAT, LPVOID)>		m_receiveCB;
	};
}
