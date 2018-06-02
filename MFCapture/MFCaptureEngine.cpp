#include "stdafx.h"
#include "MFCaptureEngine.h"


namespace MF
{
	//CaptureEngineCallback::CaptureEngineCallback(MFCaptureEngine& engine)
	//	:m_engine(engine)
	//{

	//}

	//CaptureEngineCallback::~CaptureEngineCallback()
	//{

	//}

	//HRESULT STDMETHODCALLTYPE CaptureEngineCallback::OnEvent(_In_ IMFMediaEvent *pEvent)
	//{
	//	GUID    guidType;
	//	HRESULT hrStatus;
	//	HRESULT hRes = pEvent->GetStatus(&hrStatus);
	//	if (FAILED(hRes))
	//	{
	//		hrStatus = hRes;
	//	}
	//	hRes = pEvent->GetExtendedType(&guidType);
	//	if (SUCCEEDED(hRes))
	//	{
	//		if (guidType == MF_CAPTURE_ENGINE_PREVIEW_STOPPED)
	//		{
	//			m_engine.m_event.SetEvent();
	//		}
	//		else if (guidType == MF_CAPTURE_ENGINE_RECORD_STOPPED)
	//		{
	//			m_engine.m_event.SetEvent();
	//		}
	//		else
	//		{
	//			m_engine.m_event.SetEvent();
	//		}
	//	}

	//	return S_OK;
	//}
	////////////////////////////////////////////////////////////////////////////
	//MFCaptureEngine::MFCaptureEngine()
	//{
	//	m_event.CreateEvent();
	//}

	//MFCaptureEngine::~MFCaptureEngine()
	//{
	//}
	//BOOL MFCaptureEngine::Initialize(const MFVideoCapture::Name& name)
	//{
	//	HRESULT hRes = S_OK;
	//	static const D3D_FEATURE_LEVEL levels[] = {
	//		D3D_FEATURE_LEVEL_11_1,
	//		D3D_FEATURE_LEVEL_11_0,
	//		D3D_FEATURE_LEVEL_10_1,
	//		D3D_FEATURE_LEVEL_10_0,
	//		D3D_FEATURE_LEVEL_9_3,
	//		D3D_FEATURE_LEVEL_9_2,
	//		D3D_FEATURE_LEVEL_9_1
	//	};
	//	TinyComPtr<IMFCaptureEngineClassFactory> factory;
	//	TinyComPtr<IMFAttributes> attributes;
	//	TinyComPtr<ID3D10Multithread> multithread;
	//	TinyComPtr<ID3D11DeviceContext>	context;
	//	D3D_FEATURE_LEVEL level = 0;
	//	hRes = D3D11CreateDevice(
	//		NULL,
	//		D3D_DRIVER_TYPE_HARDWARE,
	//		NULL,
	//		D3D11_CREATE_DEVICE_VIDEO_SUPPORT,
	//		levels,
	//		ARRAYSIZE(levels),
	//		D3D11_SDK_VERSION,
	//		&m_dx11,
	//		level,
	//		&context
	//	);
	//	if (FAILED(hRes))
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize D3D11CreateDevice FAIL:" << hRes;
	//		goto _ERROR;
	//	}

	//	hRes = ((*m_dx11)->QueryInterface(IID_PPV_ARGS(&multithread)));
	//	if (FAILED(hRes))
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize ID3D10Multithread FAIL:" << hRes;
	//		goto _ERROR;
	//	}
	//	multithread->SetMultithreadProtected(TRUE);
	//	hRes = MFCreateDXGIDeviceManager(&m_resetToken, &m_manager);
	//	if (FAILED(hRes))
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize MFCreateDXGIDeviceManager FAIL:" << hRes;
	//		goto _ERROR;
	//	}
	//	hRes = m_manager->ResetDevice(m_dx11, m_resetToken);
	//	if (FAILED(hRes))
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize ResetDevice FAIL:" << hRes;
	//		goto _ERROR;
	//	}
	//	hRes = MFCreateAttributes(&attributes, 1);
	//	if (FAILED(hRes))
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize MFCreateAttributes FAIL:" << hRes;
	//		goto _ERROR;
	//	}
	//	hRes = pAttributes->SetUnknown(MF_CAPTURE_ENGINE_D3D_MANAGER, m_manager);
	//	if (FAILED(hRes))
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize SetUnknown MF_CAPTURE_ENGINE_D3D_MANAGER FAIL:" << hRes;
	//		goto _ERROR;
	//	}
	//	hRes = CoCreateInstance(CLSID_MFCaptureEngineClassFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
	//	if (FAILED(hRes))
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize CoCreateInstance CLSID_MFCaptureEngineClassFactory FAIL:" << hRes;
	//		goto _ERROR;
	//	}
	//	hRes = factory->CreateInstance(CLSID_MFCaptureEngine, IID_PPV_ARGS(&m_captureEngine));
	//	if (FAILED(hRes))
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize CreateInstance CLSID_MFCaptureEngine FAIL:" << hRes;
	//		goto _ERROR;
	//	}
	//	m_captureCB = new CaptureEngineCallback(*this);
	//	if (!m_captureCB)
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize Create CaptureEngineCallback FAIL:" << E_OUTOFMEMORY;
	//		goto _ERROR;
	//	}
	//	TinyComPtr<IMFActivate> activate;
	//	if (!MFVideoCapture::GetDeviceActivate(name, &activate))
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize GetDeviceActivate FAIL";
	//		goto _ERROR;
	//	}
	//	hRes = m_captureEngine->Initialize(m_captureCB, attributes, NULL, activate);
	//	if (FAILED(hr))
	//	{
	//		LOG(ERROR) << "MFCaptureEngine Initialize CreateInstance CLSID_MFCaptureEngine FAIL:" << hRes;
	//		goto _ERROR;
	//	}
	//	return TRUE;
	//_ERROR:
	//	return FALSE;
	//}
	//void MFCaptureEngine::Uninitialize()
	//{
	//	m_dx11.Release();
	//	m_manager.Release();
	//	m_capturePreview.Release();
	//	m_captureEngine.Release();
	//	m_captureCB = NULL;
	//}
	//BOOL MFCaptureEngine::StartPreview(HWND hWND)
	//{
	//	if (!m_captureEngine)
	//		return FALSE;
	//	TinyComPtr<IMFCaptureSink> captureSink;
	//	TinyComPtr<IMFMediaType> mediaType;
	//	TinyComPtr<IMFMediaType> mediaType2;
	//	TinyComPtr<IMFCaptureSource> captureSource;
	//	HRESULT hRes = S_OK;
	//	if (!m_capturePreview)
	//	{
	//		hRes = m_captureEngine->GetSink(MF_CAPTURE_ENGINE_SINK_TYPE_PREVIEW, &captureSink);
	//		if (FAILED(hRes))
	//		{
	//			LOG(ERROR) << "MFCaptureEngine StartPreview GetSink FAIL" << hRes;
	//			goto _ERROR;
	//		}

	//		hRes = captureSink->QueryInterface(IID_PPV_ARGS(&m_capturePreview));
	//		if (FAILED(hRes))
	//		{
	//			LOG(ERROR) << "MFCaptureEngine StartPreview Get CapturePreview FAIL" << hRes;
	//			goto _ERROR;
	//		}

	//		hRes = m_capturePreview->SetRenderHandle(hWND);
	//		if (FAILED(hRes))
	//		{
	//			LOG(ERROR) << "MFCaptureEngine StartPreview SetRenderHandle FAIL" << hRes;
	//			goto _ERROR;
	//		}

	//		hRes = m_captureEngine->GetSource(&captureSource);
	//		if (FAILED(hRes))
	//		{
	//			LOG(ERROR) << "MFCaptureEngine StartPreview GetSource FAIL" << hRes;
	//			goto _ERROR;
	//		}
	//		hRes = captureSource->GetCurrentDeviceMediaType((DWORD)MF_CAPTURE_ENGINE_PREFERRED_SOURCE_STREAM_FOR_VIDEO_PREVIEW, &mediaType);
	//		if (FAILED(hRes))
	//		{
	//			LOG(ERROR) << "MFCaptureEngine StartPreview GetCurrentDeviceMediaType FAIL" << hRes;
	//			goto _ERROR;
	//		}

	//		hRes = CloneVideoMediaType(pMediaType, MFVideoFormat_RGB32, &mediaType2);
	//		if (FAILED(hRes))
	//		{
	//			LOG(ERROR) << "MFCaptureEngine StartPreview CloneVideoMediaType FAIL" << hRes;
	//			goto _ERROR;
	//		}

	//		hRes = mediaType2->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
	//		if (FAILED(hRes))
	//		{
	//			LOG(ERROR) << "MFCaptureEngine StartPreview SetUINT32 MF_MT_ALL_SAMPLES_INDEPENDENT FAIL" << hRes;
	//			goto _ERROR;
	//		}
	//		DWORD dwSinkStreamIndex;
	//		hRes = m_capturePreview->AddStream((DWORD)MF_CAPTURE_ENGINE_PREFERRED_SOURCE_STREAM_FOR_VIDEO_PREVIEW, mediaType2, NULL, &dwSinkStreamIndex);
	//		if (FAILED(hRes))
	//		{
	//			LOG(ERROR) << "MFCaptureEngine StartPreview AddStream MF_CAPTURE_ENGINE_PREFERRED_SOURCE_STREAM_FOR_VIDEO_PREVIEW FAIL" << hRes;
	//			goto _ERROR;
	//		}
	//	}
	//	hRes = m_captureEngine->StartPreview();
	//_ERROR:
	//	return SUCCEEDED(hRes);
	//}
	//BOOL MFCaptureEngine::StopPreview()
	//{
	//	if (!m_captureEngine)
	//	{
	//		LOG(ERROR) << "MFCaptureEngine StopPreview IMFCaptureEngine MF_E_NOT_INITIALIZED";
	//		return FALSE;
	//	}
	//	if (!m_capturePreview)
	//	{
	//		LOG(ERROR) << "MFCaptureEngine StopPreview IMFCapturePreviewSink MF_E_NOT_INITIALIZED";
	//		return FALSE;
	//	}
	//	return SUCCEEDED(m_captureEngine->StopPreview());
	//}
}