#include "../stdafx.h"
#include "TinyAudioDMO.h"

namespace TinyUI
{
	namespace Media
	{
		TinyAudioDMO::TinyAudioDMO()
		{

		}
		TinyAudioDMO::~TinyAudioDMO()
		{

		}
		BOOL TinyAudioDMO::Initialize(const WAVEFORMATEX* pFMT)
		{
			TinyComPtr<IUnknown> unknow;
			HRESULT hRes = unknow.CoCreateInstance(CLSID_CWMAudioAEC, NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hRes))
				return FALSE;
			hRes = unknow->QueryInterface(IID_PPV_ARGS(&m_dmo));
			if (FAILED(hRes))
				return FALSE;
			hRes = m_dmo->QueryInterface(IID_IPropertyStore, reinterpret_cast<void**>(&m_propertyStore));
			if (FAILED(hRes))
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
			if (!SetVtI4Property(m_propertyStore, MFPKEY_WMAAECMA_SYSTEM_MODE, OPTIBEAM_ARRAY_AND_AEC))
				return FALSE;
			return TRUE;
		}
		BOOL TinyAudioDMO::Open()
		{
			return TRUE;
		}
		BOOL TinyAudioDMO::Close()
		{
			return TRUE;
		}
		BOOL TinyAudioDMO::SetVtI4Property(IPropertyStore* ptrPS, REFPROPERTYKEY key, LONG value)
		{
			PROPVARIANT pv;
			PropVariantInit(&pv);
			pv.vt = VT_I4;
			pv.lVal = value;
			HRESULT hRes = ptrPS->SetValue(key, pv);
			PropVariantClear(&pv);
			if (FAILED(hRes))
			{
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyAudioDMO::SetBoolProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key, VARIANT_BOOL value)
		{
			PROPVARIANT pv;
			PropVariantInit(&pv);
			pv.vt = VT_BOOL;
			pv.boolVal = value;
			HRESULT hRes = ptrPS->SetValue(key, pv);
			PropVariantClear(&pv);
			if (FAILED(hRes))
			{
				return FALSE;
			}
			return TRUE;
		}
	};
}
