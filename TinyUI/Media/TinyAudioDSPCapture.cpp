#include "../stdafx.h"
#include "TinyAudioDSPCapture.h"

namespace TinyUI
{
	namespace Media
	{
		TinyAudioDSPCapture::TinyAudioDSPCapture()
			:m_bCapturing(FALSE),
			m_bEnableAGC(FALSE),
			m_bEnableNS(FALSE),
			m_vadMode(AEC_VAD_DISABLED),
			m_pFMT(NULL)
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

		BOOL TinyAudioDSPCapture::Open(const Name& speakName, const Name& captureName, WAVEFORMATEX* pFMT)
		{
			if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
				return FALSE;
			HRESULT hRes = CoCreateInstance(CLSID_CWMAudioAEC, NULL, CLSCTX_INPROC_SERVER, __uuidof(IMediaObject), (void**)&m_dmo);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IPropertyStore> propertyStore;
			hRes = m_dmo->QueryInterface(IID_IPropertyStore, reinterpret_cast<void**>(&propertyStore));
			if (FAILED(hRes))
				return FALSE;
			AEC_SYSTEM_MODE mode = SINGLE_CHANNEL_AEC;
			if (captureName.type() == KSNODETYPE_MICROPHONE_ARRAY)
			{
				mode = OPTIBEAM_ARRAY_AND_AEC;
			}
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_SYSTEM_MODE, mode))
				return FALSE;
			if (!SetBOOLProperty(propertyStore, MFPKEY_WMAAECMA_FEATURE_MODE, VARIANT_TRUE))
				return FALSE;
			if (!SetBOOLProperty(propertyStore, MFPKEY_WMAAECMA_FEATR_AGC, m_bEnableAGC ? VARIANT_TRUE : VARIANT_FALSE))
				return FALSE;
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_FEATR_NS, m_bEnableNS ? 1 : 0))
				return FALSE;
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_FEATR_VAD, m_vadMode))
				return FALSE;
			UINT index1 = GetDeviceIndex(eRender, speakName);
			UINT index2 = GetDeviceIndex(eCapture, captureName);
			LONG index = static_cast<ULONG>(index1 << 16) + static_cast<ULONG>(0x0000FFFF & index2);
			if (!SetVTI4Property(propertyStore, MFPKEY_WMAAECMA_DEVICE_INDEXES, index))
				return FALSE;
			hRes = MoInitMediaType(&m_mediaType, sizeof(WAVEFORMATEX));
			if (FAILED(hRes))
				return FALSE;
			m_mediaType.majortype = MEDIATYPE_Audio;
			m_mediaType.subtype = MEDIASUBTYPE_PCM;
			m_mediaType.lSampleSize = 0;
			m_mediaType.bFixedSizeSamples = TRUE;
			m_mediaType.bTemporalCompression = FALSE;
			m_mediaType.formattype = FORMAT_WaveFormatEx;
			memcpy(m_mediaType.pbFormat, pFMT, sizeof(WAVEFORMATEX));
			hRes = m_dmo->SetOutputType(0, &m_mediaType, 0);
			MoFreeMediaType(&m_mediaType);
			if (FAILED(hRes))
				return FALSE;
			m_pFMT = pFMT;
			return TRUE;
		}
		BOOL TinyAudioDSPCapture::Start()
		{
			HRESULT hRes = S_OK;
			if (m_dmo)
			{
				hRes = m_dmo->AllocateStreamingResources();
				if (FAILED(hRes))
					return FALSE;
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
			if (m_dmo)
			{
				hRes = m_dmo->FreeStreamingResources();
				if (FAILED(hRes))
					return FALSE;
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
			return m_pFMT;
		}
		void TinyAudioDSPCapture::OnMessagePump()
		{
			WAVEFORMATEX* pFMT = GetFormat();
			TinyScopedAvrt avrt("Pro Audio");
			avrt.SetPriority();
			TinyScopedReferencePtr<MediaBuffer> mediaBuffer(new MediaBuffer(pFMT->nSamplesPerSec * pFMT->nBlockAlign));
			m_dmoBuffer.pBuffer = mediaBuffer;
			HRESULT hRes = S_OK;
			BOOL bCapturing = TRUE;
			while (bCapturing)
			{
				switch (WaitForSingleObject(m_audioStop, 5))
				{
				case WAIT_FAILED:
				case WAIT_ABANDONED:
				case WAIT_OBJECT_0:
					bCapturing = FALSE;
					break;
				case WAIT_TIMEOUT:
					break;
				default:
					bCapturing = FALSE;
					break;
				}
				DWORD dwStatus = 0;
				do
				{
					m_dmoBuffer.dwStatus = 0;
					hRes = m_dmo->ProcessOutput(0, 1, &m_dmoBuffer, &dwStatus);
					dwStatus = m_dmoBuffer.dwStatus;
					DWORD cbProduced = 0;
					if (FAILED(hRes))
					{
						bCapturing = FALSE;
						break;
					}
					BYTE* data;
					mediaBuffer->GetBufferAndLength(&data, &cbProduced);
					OnDataAvailable(data, cbProduced, this);
					mediaBuffer->SetLength(0);
				} while (DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE & dwStatus);
			}
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
