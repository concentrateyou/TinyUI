#pragma once
#include "MFCommon.h"
#include "MFVideoCapture.h"

namespace MF
{
	/*class MFCaptureEngine;

	class CaptureEngineCallback : public TinyReference<CaptureEngineCallback>, public IMFCaptureEngineOnEventCallback
	{
		DISALLOW_COPY_AND_ASSIGN(CaptureEngineCallback)
	public:
		CaptureEngineCallback(MFCaptureEngine& engine);
		virtual ~CaptureEngineCallback();
	public:
		HRESULT STDMETHODCALLTYPE OnEvent(_In_ IMFMediaEvent *pEvent) OVERRIDE;
	private:
		MFCaptureEngine& m_engine;
	};

	class MFCaptureEngine
	{
		DISALLOW_COPY_AND_ASSIGN(MFCaptureEngine)
	public:
		MFCaptureEngine();
		~MFCaptureEngine();
		BOOL Initialize(const MFVideoCapture::Name& name);
		void Uninitialize();
		BOOL StartPreview(HWND hWND);
		BOOL StopPreview();
	private:
		UINT											m_resetToken;
		TinyEvent										m_event;
		TinyComPtr<ID3D11Device>						m_dx11;
		TinyComPtr<IMFDXGIDeviceManager>				m_manager;
		TinyComPtr<IMFCaptureEngine>					m_captureEngine;
		TinyComPtr<IMFCapturePreviewSink>				m_capturePreview;
		TinyScopedReferencePtr<CaptureEngineCallback>	m_captureCB;
	};*/
}



