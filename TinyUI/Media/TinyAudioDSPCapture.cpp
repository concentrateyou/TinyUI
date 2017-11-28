#include "../stdafx.h"
#include "TinyAudioDSPCapture.h"
#include "../Common/TinyLogging.h"

namespace TinyUI
{
	namespace Media
	{
		TinyAudioDSPCapture::TinyAudioDSPCapture()
			:m_bCapturing(FALSE),
			m_bEnableAGC(FALSE),
			m_bEnableNS(FALSE),
			m_vadMode(AEC_VAD_DISABLED)
		{
			m_audioStop.CreateEvent(FALSE, FALSE, NULL, NULL);
		}
		TinyAudioDSPCapture::~TinyAudioDSPCapture()
		{

		}

		void TinyAudioDSPCapture::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, size, lpParameter);
			}
		}
		void TinyAudioDSPCapture::Initialize()
		{

		}
		void TinyAudioDSPCapture::Initialize(Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			m_callback = std::move(callback);
		}
		BOOL TinyAudioDSPCapture::Open(const GUID& speakGUID, const GUID& captureGUID, WAVEFORMATEX* pFMT)
		{
			TinyComPtr<IPropertyStore> propertyStore;
			INT index1 = GetDeviceIndex(eRender, speakGUID);
			INT index2 = GetDeviceIndex(eCapture, captureGUID);
			LONG index = static_cast<ULONG>(index1 << 16) + static_cast<ULONG>(0x0000FFFF & index2);
			if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
				return FALSE;
			HRESULT hRes = CoCreateInstance(CLSID_CWMAudioAEC, NULL, CLSCTX_INPROC_SERVER, __uuidof(IMediaObject), (void**)&m_dmo);
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open CoCreateInstance FAIL";
				goto _ERROR;
			}
			hRes = m_dmo->QueryInterface(IID_IPropertyStore, reinterpret_cast<void**>(&propertyStore));
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open QueryInterface propertyStore FAIL";
				goto _ERROR;
			}
			BOOL IsMA = FALSE;
			AEC_SYSTEM_MODE mode = SINGLE_CHANNEL_AEC;
			if (!IsMicrophoneArray(eCapture, captureGUID, IsMA))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open IsMicrophoneArray False";
				goto _ERROR;
			}
			if (IsMA)
				mode = OPTIBEAM_ARRAY_AND_AEC;
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_SYSTEM_MODE, mode))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_SYSTEM_MODE FAIL";
				goto _ERROR;
			}
			if (!SetBOOLProperty(propertyStore, MFPKEY_WMAAECMA_FEATURE_MODE, VARIANT_TRUE))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_FEATURE_MODE FAIL";
				goto _ERROR;
			}
			if (!SetBOOLProperty(propertyStore, MFPKEY_WMAAECMA_FEATR_AGC, m_bEnableAGC ? VARIANT_TRUE : VARIANT_FALSE))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_FEATR_AGC FAIL";
				goto _ERROR;
			}
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_FEATR_NS, m_bEnableNS ? 1 : 0))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_FEATR_NS FAIL";
				goto _ERROR;
			}
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_FEATR_VAD, m_vadMode))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_FEATR_VAD FAIL";
				goto _ERROR;
			}
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_DEVICE_INDEXES, index))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_DEVICE_INDEXES FAIL";
				goto _ERROR;
			}
			hRes = MoInitMediaType(&m_mediaType, sizeof(WAVEFORMATEX));
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] MoInitMediaType:" << hRes;
				goto _ERROR;
			}
			m_mediaType.majortype = MEDIATYPE_Audio;
			m_mediaType.subtype = MEDIASUBTYPE_PCM;
			m_mediaType.lSampleSize = 0;
			m_mediaType.bFixedSizeSamples = TRUE;
			m_mediaType.bTemporalCompression = FALSE;
			m_mediaType.formattype = FORMAT_WaveFormatEx;
			m_mediaType.cbFormat = sizeof(WAVEFORMATEX);
			memcpy(m_mediaType.pbFormat, pFMT, sizeof(WAVEFORMATEX));
			hRes = m_dmo->SetOutputType(0, &m_mediaType, 0);
			MoFreeMediaType(&m_mediaType);
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] DMO SetOutputType:" << hRes;
				goto _ERROR;
			}
			if (pFMT != NULL)
			{
				m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX) + pFMT->cbSize]);
				memcpy(m_waveFMT, (BYTE*)pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			}
			return TRUE;
		_ERROR:
			m_dmo.Release();
			return FALSE;
		}
		BOOL TinyAudioDSPCapture::Open(const Name& speakName, const Name& captureName, WAVEFORMATEX* pFMT)
		{
			AEC_SYSTEM_MODE mode = SINGLE_CHANNEL_AEC;
			INT index1 = GetDeviceIndex(eRender, speakName);
			INT index2 = GetDeviceIndex(eCapture, captureName);;
			LONG index = static_cast<ULONG>(index1 << 16) + static_cast<ULONG>(0x0000FFFF & index2);
			TinyComPtr<IPropertyStore> propertyStore;
			if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
				return FALSE;
			HRESULT hRes = CoCreateInstance(CLSID_CWMAudioAEC, NULL, CLSCTX_INPROC_SERVER, __uuidof(IMediaObject), (void**)&m_dmo);
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open CoCreateInstance FAIL";
				goto _ERROR;
			}
			hRes = m_dmo->QueryInterface(IID_IPropertyStore, reinterpret_cast<void**>(&propertyStore));
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open QueryInterface propertyStore FAIL";
				goto _ERROR;
			}
			if (captureName.type() == KSNODETYPE_MICROPHONE_ARRAY ||
				captureName.type() == KSNODETYPE_PROCESSING_MICROPHONE_ARRAY)
			{
				mode = OPTIBEAM_ARRAY_AND_AEC;
			}
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_SYSTEM_MODE, mode))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_SYSTEM_MODE FAIL";
				goto _ERROR;
			}
			if (!SetBOOLProperty(propertyStore, MFPKEY_WMAAECMA_FEATURE_MODE, VARIANT_TRUE))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_FEATURE_MODE FAIL";
				goto _ERROR;
			}
			if (!SetBOOLProperty(propertyStore, MFPKEY_WMAAECMA_FEATR_AGC, m_bEnableAGC ? VARIANT_TRUE : VARIANT_FALSE))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_FEATR_AGC FAIL";
				goto _ERROR;
			}
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_FEATR_NS, m_bEnableNS ? 1 : 0))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_FEATR_NS FAIL";
				goto _ERROR;
			}
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_FEATR_VAD, m_vadMode))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_FEATR_VAD FAIL";
				goto _ERROR;
			}
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_DEVICE_INDEXES, index))
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] Open Set MFPKEY_WMAAECMA_DEVICE_INDEXES FAIL";
				goto _ERROR;
			}
			hRes = MoInitMediaType(&m_mediaType, sizeof(WAVEFORMATEX));
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] MoInitMediaType:" << hRes;
				goto _ERROR;
			}
			m_mediaType.majortype = MEDIATYPE_Audio;
			m_mediaType.subtype = MEDIASUBTYPE_PCM;
			m_mediaType.lSampleSize = 0;
			m_mediaType.bFixedSizeSamples = TRUE;
			m_mediaType.bTemporalCompression = FALSE;
			m_mediaType.formattype = FORMAT_WaveFormatEx;
			m_mediaType.cbFormat = sizeof(WAVEFORMATEX);
			memcpy(m_mediaType.pbFormat, pFMT, sizeof(WAVEFORMATEX));
			hRes = m_dmo->SetOutputType(0, &m_mediaType, 0);
			MoFreeMediaType(&m_mediaType);
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[TinyAudioDSPCapture] DMO SetOutputType:" << hRes;
				goto _ERROR;
			}
			if (pFMT != NULL)
			{
				m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX) + pFMT->cbSize]);
				memcpy(m_waveFMT, (BYTE*)pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			}
			return TRUE;
		_ERROR:
			m_dmo.Release();
			return FALSE;
		}
		BOOL TinyAudioDSPCapture::Start()
		{
			HRESULT hRes = S_OK;
			if (m_dmo != NULL)
			{
				hRes = m_dmo->AllocateStreamingResources();
				if (hRes != S_OK)
				{
					LOG(ERROR) << "[TinyAudioDSPCapture] DMO AllocateStreamingResources:" << hRes;
					return FALSE;
				}
			}
			m_audioStop.ResetEvent();
			m_task.Close(INFINITE);
			if (!m_task.Submit(BindCallback(&TinyAudioDSPCapture::OnMessagePump, this)))
				return FALSE;
			m_bCapturing = TRUE;
			return TRUE;
		}
		BOOL TinyAudioDSPCapture::Stop()
		{
			HRESULT hRes = S_OK;
			if (m_dmo != NULL)
			{
				hRes = m_dmo->FreeStreamingResources();
				if (hRes != S_OK)
				{
					LOG(ERROR) << "[TinyAudioDSPCapture] DMO FreeStreamingResources:" << hRes;
					return FALSE;
				}
			}
			m_audioStop.SetEvent();
			m_task.Close(INFINITE);
			m_bCapturing = FALSE;
			return TRUE;
		}
		BOOL TinyAudioDSPCapture::Reset()
		{
			return FALSE;
		}
		BOOL TinyAudioDSPCapture::Close()
		{
			if (m_bCapturing && !Stop())
			{
				m_dmo.Release();
				return FALSE;
			}
			m_dmo.Release();
			m_bCapturing = FALSE;
			return TRUE;
		}
		void TinyAudioDSPCapture::EnableAGC(BOOL bAllow)
		{
			m_bEnableAGC = bAllow;
		}
		void TinyAudioDSPCapture::EnableNS(BOOL bAllow)
		{
			m_bEnableNS = bAllow;
		}
		void TinyAudioDSPCapture::SetVADMode(AEC_VAD_MODE mode)
		{
			m_vadMode = mode;

		}
		WAVEFORMATEX* TinyAudioDSPCapture::GetFormat() const
		{
			return reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
		}
		BOOL TinyAudioDSPCapture::IsCapturing() const
		{
			return m_bCapturing;
		}
		BOOL TinyAudioDSPCapture::IsEmpty()
		{
			return m_dmo == NULL;
		}
		void TinyAudioDSPCapture::OnMessagePump()
		{
			WAVEFORMATEX* pFMT = GetFormat();
			TinyScopedAvrt avrt("Pro Audio");
			avrt.SetPriority();
			TinyScopedReferencePtr<IMediaBuffer> mediaBuffer(new MediaBuffer(pFMT->nSamplesPerSec * pFMT->nBlockAlign));
			m_dmoBuffer.pBuffer = mediaBuffer;
			HRESULT hRes = S_OK;
			BOOL bCapturing = TRUE;
			while (bCapturing)
			{
				switch (WaitForSingleObject(m_audioStop, MILLISECONDS_TO_VISUALIZE / 2))
				{
				case WAIT_FAILED:
				case WAIT_ABANDONED:
				case WAIT_OBJECT_0:
					bCapturing = FALSE;
				case WAIT_TIMEOUT:
					break;
				default:
					bCapturing = FALSE;
					break;
				}
				DWORD dwStatus = 0;
				do
				{
					if (!bCapturing)
						break;
					m_dmoBuffer.dwStatus = 0;
					hRes = m_dmo->ProcessOutput(0, 1, &m_dmoBuffer, &dwStatus);
					if (FAILED(hRes) && hRes != WMAAECMA_E_NO_ACTIVE_RENDER_STREAM)
					{
						TRACE("DMP ProcessOutput FAIL\n");
						LOG(ERROR) << "DMP ProcessOutput:" << hRes << " FAIL";
						bCapturing = FALSE;
						m_bCapturing = FALSE;
						break;
					}
					dwStatus = m_dmoBuffer.dwStatus;
					DWORD cbProduced = 0;
					BYTE* data;
					mediaBuffer->GetBufferAndLength(&data, &cbProduced);
					if (cbProduced > 0)
					{
						OnDataAvailable(data, cbProduced, this);
					}
					mediaBuffer->SetLength(0);
				} while (DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE & dwStatus);
			}
			LOG(INFO) << "OnMessagePump EXIT";
		}
		BOOL TinyAudioDSPCapture::SetVTI4Property(IPropertyStore* ptrPS, REFPROPERTYKEY key, LONG value)
		{
			PROPVARIANT pv;
			PropVariantInit(&pv);
			pv.vt = VT_I4;
			pv.lVal = value;
			HRESULT hRes = ptrPS->SetValue(key, pv);
			hRes = ptrPS->GetValue(key, &pv);
			PropVariantClear(&pv);
			return hRes == S_OK;
		}
		BOOL TinyAudioDSPCapture::SetBOOLProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key, VARIANT_BOOL value)
		{
			PROPVARIANT pv;
			PropVariantInit(&pv);
			pv.vt = VT_BOOL;
			pv.boolVal = value;
			HRESULT hRes = ptrPS->SetValue(key, pv);
			hRes = ptrPS->GetValue(key, &pv);
			PropVariantClear(&pv);
			return hRes == S_OK;
		}
	};
}
