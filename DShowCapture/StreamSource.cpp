#include "stdafx.h"
#include "StreamSource.h"
#include "SourceStream.h"

namespace Media
{
	StreamSource::StreamSource(REFCLSID  clsid)
		:FilterBase(clsid, &m_lock),
		m_iPins(0),
		m_ppStreams(NULL)
	{

	}


	StreamSource::~StreamSource()
	{
		while (m_iPins != 0) 
		{
			SAFE_DELETE(m_ppStreams[m_iPins-1]);
		}
		ASSERT(m_ppStreams == NULL);
	}

	INT StreamSource::GetPinCount()
	{
		TinyAutoLock lock(m_lock);
		return m_iPins;
	}

	IPin* StreamSource::GetPin(INT index)
	{
		TinyAutoLock lock(m_lock);
		if ((index >= 0) && (index < m_iPins))
		{
			ASSERT(m_ppStreams[index]);
			return m_ppStreams[index];
		}
		return NULL;
	}

	HRESULT STDMETHODCALLTYPE StreamSource::FindPin(LPCWSTR Id, _Out_ IPin **ppPin)
	{
		CheckPointer(ppPin, E_POINTER);
		INT i = _wtoi(Id) - 1;
		*ppPin = GetPin(i);
		if (*ppPin != NULL)
		{
			(*ppPin)->AddRef();
			return NOERROR;
		}
		else
		{
			return VFW_E_NOT_FOUND;
		}
	}
}