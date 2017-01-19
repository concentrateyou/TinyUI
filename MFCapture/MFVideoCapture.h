#pragma once
#include "MFCommon.h"
#include "MFObserver.h"
#include "MFReaderCallback.h"
#include "MFVideoCaptureParam.h"

namespace MF
{
	class MFVideoCapture : public MFObserver
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
		MFVideoCapture();
		virtual ~MFVideoCapture();
		BOOL	Initialize(const Name& name);
		BOOL	Initialize(const Name& name, Callback<void(BYTE*, LONG, FLOAT, LPVOID)>&& receiveCB);
		void	Uninitialize();
	public:
		virtual BOOL Allocate(const MFVideoCaptureParam& param);
		virtual void Deallocate();
		void	OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) OVERRIDE;
	public:
		static BOOL GetFormat(const GUID& guid, VideoPixelFormat* format);
		static BOOL GetDevices(vector<MFVideoCapture::Name>& names);
		static BOOL GetDeviceParams(const MFVideoCapture::Name& device, vector<MFVideoCaptureParam>& params);
		static BOOL GetDeviceSource(const MFVideoCapture::Name& device, IMFMediaSource** source);
	protected:
		BOOL										m_bCapturing;
		TinyScopedReferencePtr<MFReaderCallback>	m_readerCB;
		TinyComPtr<IMFSourceReader>					m_reader;
		TinyComPtr<IMFMediaSource>					m_source;
		Callback<void(BYTE*, LONG, FLOAT, LPVOID)>	m_receiveCB;
	};
}
