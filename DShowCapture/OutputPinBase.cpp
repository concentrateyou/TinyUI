#include "stdafx.h"
#include "OutputPinBase.h"

namespace DShow
{
	OutputPinBase::OutputPinBase(FilterBase* pFilter, WCHAR* pzName, TinyLock* lock)
		:PinBase(pFilter, PINDIR_OUTPUT, pzName, lock)
	{
	}
	OutputPinBase::~OutputPinBase()
	{
	}
	HRESULT OutputPinBase::CheckConnect(IPin *pPin)
	{
		HRESULT hRes = PinBase::CheckConnect(pPin);
		if (hRes != NOERROR)
			return hRes;
		hRes = pPin->QueryInterface(IID_IMemInputPin, (void **)&m_inputPin);
		if (hRes != NOERROR)
			return hRes;
		return NOERROR;
	}
	HRESULT OutputPinBase::OnConnect(IPin *pReceivePin)
	{
		UNREFERENCED_PARAMETER(pReceivePin);
		return GetAllocator(m_inputPin, &m_allocator);
	}
	HRESULT OutputPinBase::OnDisconnect()
	{
		if (m_allocator)
		{
			HRESULT hRes = m_allocator->Decommit();
			if (hRes != S_OK)
			{
				return hRes;
			}
			m_allocator.Release();
		}
		m_inputPin.Release();
		return NOERROR;
	}
	HRESULT OutputPinBase::OnActive(BOOL active)
	{
		if (!m_allocator)
		{
			return VFW_E_NO_ALLOCATOR;
		}
		if (active)
		{
			return m_allocator->Commit();
		}
		else
		{
			return m_allocator->Decommit();
		}
	}
	HRESULT STDMETHODCALLTYPE OutputPinBase::EndOfStream(void)
	{
		if (!m_connector)
		{
			return VFW_E_NOT_CONNECTED;
		}
		return m_connector->EndOfStream();
	}
	HRESULT STDMETHODCALLTYPE OutputPinBase::BeginFlush(void)
	{
		if (!m_connector)
		{
			return VFW_E_NOT_CONNECTED;
		}
		return m_connector->BeginFlush();
	}
	HRESULT STDMETHODCALLTYPE OutputPinBase::EndFlush(void)
	{
		if (!m_connector)
		{
			return VFW_E_NOT_CONNECTED;
		}
		return m_connector->EndFlush();
	}
	HRESULT STDMETHODCALLTYPE OutputPinBase::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
	{
		if (!m_connector)
		{
			return VFW_E_NOT_CONNECTED;
		}
		return m_connector->NewSegment(tStart, tStop, dRate);
	}
	HRESULT OutputPinBase::InitializeAllocator(IMemAllocator **ppAlloc)
	{
		return CoCreateInstance(CLSID_MemoryAllocator,
			0,
			CLSCTX_INPROC_SERVER,
			IID_IMemAllocator,
			(void **)ppAlloc);
	}
	HRESULT OutputPinBase::GetAllocator(IMemInputPin * pPin, IMemAllocator ** ppAlloc)
	{
		HRESULT hRes = NOERROR;
		*ppAlloc = NULL;
		ALLOCATOR_PROPERTIES prop;
		ZeroMemory(&prop, sizeof(prop));
		pPin->GetAllocatorRequirements(&prop);
		if (prop.cbAlign == 0)
		{
			prop.cbAlign = 1;
		}
		hRes = pPin->GetAllocator(ppAlloc);
		if (SUCCEEDED(hRes))
		{

			hRes = Allocate(*ppAlloc, &prop);
			if (SUCCEEDED(hRes))
			{
				hRes = pPin->NotifyAllocator(*ppAlloc, FALSE);
				if (SUCCEEDED(hRes))
				{
					return NOERROR;
				}
			}
		}
		if (*ppAlloc)
		{
			(*ppAlloc)->Release();
			*ppAlloc = NULL;
		}
		hRes = CoCreateInstance(CLSID_MemoryAllocator,
			0,
			CLSCTX_INPROC_SERVER,
			IID_IMemAllocator,
			(void **)ppAlloc);
		if (SUCCEEDED(hRes))
		{
			hRes = Allocate(*ppAlloc, &prop);
			if (SUCCEEDED(hRes))
			{
				hRes = pPin->NotifyAllocator(*ppAlloc, FALSE);
				if (SUCCEEDED(hRes))
				{
					return NOERROR;
				}
			}
		}
		if (*ppAlloc)
		{
			(*ppAlloc)->Release();
			*ppAlloc = NULL;
		}
		return hRes;
	}
	HRESULT OutputPinBase::GetSample(IMediaSample ** ppSample, REFERENCE_TIME * pStartTime, REFERENCE_TIME * pEndTime, DWORD dwFlags)
	{
		if (m_allocator)
		{
			return m_allocator->GetBuffer(ppSample, pStartTime, pEndTime, dwFlags);
		}
		return E_NOINTERFACE;
	}
	HRESULT OutputPinBase::FillSample(IMediaSample *pSample)
	{
		if (!m_inputPin)
		{
			return VFW_E_NOT_CONNECTED;
		}
		return m_inputPin->Receive(pSample);
	}
}