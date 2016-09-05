#include "stdafx.h"
#include "FilterBase.h"
#include "PinBase.h"
#include "TypeEnumerator.h"

namespace Media
{
	PinBase::PinBase(FilterBase* pFilter, PIN_DIRECTION dir, WCHAR* pzName)
		:m_pFilter(pFilter),
		m_dir(dir),
		m_pzName(pzName),
		m_bFlushing(FALSE),
		m_startTime(0),
		m_stopTime(0),
		m_rate(0.0F)
	{
		ZeroMemory((void*)&m_mediaType, sizeof(m_mediaType));
	}
	PinBase::~PinBase()
	{
		FreeMediaType(m_mediaType);
	}
	HRESULT PinBase::CheckConnect(IPin *pPin)
	{
		PIN_DIRECTION pd;
		pPin->QueryDirection(&pd);
		ASSERT((pd == PINDIR_OUTPUT) || (pd == PINDIR_INPUT));
		ASSERT((m_dir == PINDIR_OUTPUT) || (m_dir == PINDIR_INPUT));
		if (pd == m_dir)
		{
			return VFW_E_INVALID_DIRECTION;
		}
		return NOERROR;
	}
	HRESULT PinBase::OnDisconnect()
	{
		return NOERROR;
	}
	HRESULT	PinBase::OnActive(BOOL active)
	{
		return NOERROR;
	}
	HRESULT PinBase::OnRun(REFERENCE_TIME tStart)
	{
		return NOERROR;
	}
	HRESULT PinBase::OnConnect(IPin *pReceivePin)
	{
		return NOERROR;
	}
	BOOL PinBase::IsConnected(void) const
	{
		return m_connector != NULL;
	}
	IPin* PinBase::GetConnector()
	{
		return m_connector;
	}
	BOOL PinBase::IsFlushing() const
	{
		return m_bFlushing;
	}
	REFERENCE_TIME PinBase::GetCurrentStartTime() const
	{
		return m_startTime;
	}
	REFERENCE_TIME PinBase::GetCurrentStopTime() const
	{
		return m_stopTime;
	}
	HRESULT PinBase::SetMediaType(const AM_MEDIA_TYPE *mediaType)
	{
		return CopyMediaType(&m_mediaType, mediaType);
	}
	HRESULT PinBase::AgreeMediaType(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt)
	{
		ASSERT(pReceivePin);
		IEnumMediaTypes *pEnumMediaTypes = NULL;
		if ((pmt != NULL) &&
			(pmt->majortype != GUID_NULL) &&
			(pmt->formattype != GUID_NULL))
		{
			return TryConnection(pReceivePin, pmt);
		}
		HRESULT hrFailure = VFW_E_NO_ACCEPTABLE_TYPES;
		for (INT i = 0; i < 2; i++)
		{
			HRESULT hRes;
			if (i == 0)
			{
				hRes = pReceivePin->EnumMediaTypes(&pEnumMediaTypes);
			}
			else
			{
				hRes = EnumMediaTypes(&pEnumMediaTypes);
			}
			if (SUCCEEDED(hRes))
			{
				ASSERT(pEnumMediaTypes);
				hRes = TryMediaTypes(pReceivePin, pmt, pEnumMediaTypes);
				pEnumMediaTypes->Release();
				if (SUCCEEDED(hRes))
				{
					return NOERROR;
				}
				else
				{
					if ((hRes != E_FAIL) &&
						(hRes != E_INVALIDARG) &&
						(hRes != VFW_E_TYPE_NOT_ACCEPTED))
					{
						hrFailure = hRes;
					}
				}
			}
		}
		return hrFailure;
	}
	HRESULT PinBase::TryConnection(IPin* pReceivePin, const AM_MEDIA_TYPE* pmt)
	{
		HRESULT hRes = CheckConnect(pReceivePin);
		if (hRes != NOERROR)
		{
			return S_FALSE;
		}
		hRes = CheckMediaType(pmt);
		if (hRes == NOERROR)
		{
			m_connector = pReceivePin;
			m_connector->AddRef();
			hRes = SetMediaType(pmt);
			if (SUCCEEDED(hRes))
			{
				hRes = pReceivePin->ReceiveConnection((IPin *)this, pmt);
				if (SUCCEEDED(hRes))
				{
					OnConnect(pReceivePin);
					return NOERROR;
				}
				else
				{
					pReceivePin->Disconnect();
				}
			}
		}
		else
		{
			if (SUCCEEDED(hRes) ||
				(hRes == E_FAIL) ||
				(hRes == E_INVALIDARG))
			{
				hRes = VFW_E_TYPE_NOT_ACCEPTED;
			}
		}
		OnDisconnect();
		if (m_connector)
		{
			m_connector.Release();
		}
		return hRes;
	}
	HRESULT PinBase::TryMediaTypes(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt, IEnumMediaTypes *pEnum)
	{
		HRESULT hRes = pEnum->Reset();
		if (hRes != NOERROR)
		{
			return hRes;
		}
		AM_MEDIA_TYPE *pMediaType = NULL;
		ULONG ulMediaCount = 0;
		HRESULT hrFailure = NOERROR;
		for (;;)
		{
			hRes = pEnum->Next(1, (AM_MEDIA_TYPE**)&pMediaType, &ulMediaCount);
			if (hRes != NOERROR)
			{
				if (NOERROR == hrFailure)
				{
					hrFailure = VFW_E_NO_ACCEPTABLE_TYPES;
				}
				return hrFailure;
			}
			ASSERT(ulMediaCount == 1);
			ASSERT(pMediaType);
			if (pMediaType && ((pmt == NULL) || MediaTypeMatchPartial(pMediaType, pmt)))
			{
				hRes = TryConnection(pReceivePin, pMediaType);
				if (FAILED(hRes) &&
					SUCCEEDED(hrFailure) &&
					(hRes != E_FAIL) &&
					(hRes != E_INVALIDARG) &&
					(hRes != VFW_E_TYPE_NOT_ACCEPTED))
				{
					hrFailure = hRes;
				}
			}
			else
			{
				hRes = VFW_E_NO_ACCEPTABLE_TYPES;
			}

			if (pMediaType)
			{
				DeleteMediaType(pMediaType);
				pMediaType = NULL;
			}
			if (NOERROR == hRes)
			{
				return hRes;
			}
		}
	}
	BOOL PinBase::MediaTypeMatchPartial(const AM_MEDIA_TYPE *pmt1, const AM_MEDIA_TYPE *pmt2)
	{
		if ((pmt1->majortype != GUID_NULL) &&
			(pmt2->majortype != GUID_NULL) &&
			(pmt2->majortype != pmt1->majortype))
		{
			return FALSE;
		}
		if ((pmt1->subtype != GUID_NULL) &&
			(pmt2->subtype != GUID_NULL) &&
			(pmt1->subtype != pmt2->subtype))
		{
			return FALSE;
		}

		if (pmt1->formattype != GUID_NULL &&
			pmt2->formattype != GUID_NULL)
		{
			if (pmt1->formattype != pmt2->formattype)
			{
				return FALSE;
			}
			if (pmt1->cbFormat != pmt2->cbFormat)
			{
				return FALSE;
			}
			if ((pmt1->cbFormat != 0) && (memcmp(pmt1->pbFormat, pmt2->pbFormat, pmt1->cbFormat) != 0))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	HRESULT STDMETHODCALLTYPE PinBase::Connect(IPin *pReceivePin, _In_opt_ const AM_MEDIA_TYPE *pmt)
	{
		if (!pReceivePin || !pmt)
		{
			return E_POINTER;
		}
		if (m_connector)
		{
			return VFW_E_ALREADY_CONNECTED;
		}
		HRESULT hRes = AgreeMediaType(pReceivePin, pmt);
		if (hRes != NOERROR)
			return hRes;
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt)
	{
		VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
		WCHAR str[39];
		StringFromGUID2(pmt->subtype, str, 39);
		TRACE("ReceiveConnection-subtype:%s,cx:%d,cy:%d\n", UTF16ToUTF8(str).c_str(), h->bmiHeader.biWidth, h->bmiHeader.biHeight);
		if (!pConnector || !pmt)
		{
			return E_POINTER;
		}
		if (m_connector)
		{
			return VFW_E_ALREADY_CONNECTED;
		}
		HRESULT hRes = CheckConnect(pConnector);
		if (hRes != NOERROR)
		{
			ASSERT(OnDisconnect() == NOERROR);
			return hRes;
		}

		hRes = CheckMediaType(pmt);
		if (hRes != NOERROR)
		{
			ASSERT(OnDisconnect() == NOERROR);
			if (SUCCEEDED(hRes) || (hRes == E_FAIL) || (hRes == E_INVALIDARG))
			{
				hRes = VFW_E_TYPE_NOT_ACCEPTED;
			}
			return hRes;
		}
		m_connector = pConnector;
		hRes = SetMediaType(pmt);
		if (hRes == S_OK)
		{
			OnConnect(pConnector);
			return hRes;
		}
		m_connector->Release();
		m_connector = NULL;
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE PinBase::Disconnect(void)
	{
		if (!m_connector)
			return S_FALSE;
		ASSERT(OnDisconnect() == NOERROR);
		m_connector.Release();
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::ConnectedTo(_Out_ IPin **ppPin)
	{
		IPin *pPin = m_connector;
		*ppPin = pPin;
		if (!m_connector)
			return VFW_E_NOT_CONNECTED;
		m_connector->AddRef();
		TRACE("ConnectedTo : OK\n");
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::ConnectionMediaType(_Out_ AM_MEDIA_TYPE *pmt)
	{
		if (!m_connector)
		{
			ZeroMemory(pmt, sizeof(AM_MEDIA_TYPE));
			pmt->lSampleSize = 1;
			pmt->bFixedSizeSamples = TRUE;
			return VFW_E_NOT_CONNECTED;
		}
		CopyMediaType(pmt, &m_mediaType);
		WCHAR str[39];
		StringFromGUID2(m_mediaType.subtype, str, 39);
		TRACE("ConnectionMediaType subtype:%s\n", UTF16ToUTF8(str).c_str());
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryPinInfo(_Out_ PIN_INFO *pInfo)
	{
		pInfo->pFilter = m_pFilter;
		if (m_pFilter)
		{
			m_pFilter->AddRef();
		}
		if (m_pzName)
		{
			StringCchCopyW(pInfo->achName, ARRAYSIZE(pInfo->achName), m_pzName);
		}
		else
		{
			pInfo->achName[0] = L'\0';
		}
		pInfo->dir = m_dir;
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryDirection(_Out_ PIN_DIRECTION *pPinDir)
	{
		TRACE("QueryDirection\n");
		*pPinDir = m_dir;
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryId(_Out_ LPWSTR *Id)
	{
		size_t size;
		HRESULT hRes = StringCbLengthW(m_pzName, 100000, &size);
		if (hRes != NOERROR)
			return hRes;
		*Id = (LPWSTR)CoTaskMemAlloc(size + sizeof(WCHAR));
		if (*Id == NULL)
		{
			return E_OUTOFMEMORY;
		}
		CopyMemory(*Id, m_pzName, size + sizeof(WCHAR));
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryAccept(const AM_MEDIA_TYPE *pmt)
	{
		HRESULT hRes = CheckMediaType(pmt);
		if (hRes != NOERROR)
		{
			return S_FALSE;
		}
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::EnumMediaTypes(_Out_ IEnumMediaTypes **ppEnum)
	{
		if (*ppEnum = new TypeEnumerator(this))
		{
			(*ppEnum)->AddRef();
			return NOERROR;
		}
		return E_OUTOFMEMORY;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryInternalConnections(_Out_writes_to_opt_(*nPin, *nPin) IPin **apPin, ULONG *nPin)
	{
		return E_NOTIMPL;
	}
	HRESULT STDMETHODCALLTYPE PinBase::EndOfStream(void)
	{
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::BeginFlush(void)
	{
		m_bFlushing = TRUE;
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::EndFlush(void)
	{
		m_bFlushing = FALSE;
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
	{
		m_startTime = tStart;
		m_stopTime = tStop;
		m_rate = dRate;
		return NOERROR;
	}
	HRESULT STDMETHODCALLTYPE PinBase::QueryInterface(REFIID riid, void **ppvObject)
	{
		if (IsEqualIID(riid, IID_IPin) || IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObject = static_cast<IPin*>(this);
		}
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		AddRef();
		return NOERROR;
	}
	ULONG STDMETHODCALLTYPE PinBase::AddRef(void)
	{
		TinyReference < PinBase >::AddRef();
		return TinyReference < PinBase >::GetReference();
	}
	ULONG STDMETHODCALLTYPE PinBase::Release(void)
	{
		TinyReference < PinBase >::Release();
		return TinyReference < PinBase >::GetReference();
	}
}