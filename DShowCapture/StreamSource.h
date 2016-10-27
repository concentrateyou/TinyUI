#pragma once
#include "DShowCommon.h"
#include "FilterBase.h"

namespace DShow
{
	class SourceStream;
	/// <summary>
	/// Á÷µÄFilter
	/// </summary>
	class StreamSource : public FilterBase
	{
	public:
		StreamSource(REFCLSID  clsid);
		virtual ~StreamSource();
		INT GetPinCount() OVERRIDE;
		IPin* GetPin(INT index) OVERRIDE;
		HRESULT STDMETHODCALLTYPE FindPin(LPCWSTR Id, _Out_ IPin **ppPin) OVERRIDE;
	public:
		HRESULT   AddPin(SourceStream *);
		HRESULT   RemovePin(SourceStream *);
	protected:
		TinyLock		m_lock;
		INT				m_iPins;
		SourceStream**	m_ppStreams;
	};
}
