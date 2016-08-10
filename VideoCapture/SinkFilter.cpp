#include "stdafx.h"
#include "SinkFilter.h"
#include "SinkInputPin.h"

namespace Media
{

	SinkFilter::SinkFilter(FilterObserver* observer)
	{
		m_sinkInputPin = new SinkInputPin(this, observer);
	}

	SinkFilter::~SinkFilter()
	{
		m_sinkInputPin->SetOwner(NULL);
	}

	void SinkFilter::SetRequestedParam(const VideoCaptureParam& param)
	{
		m_sinkInputPin->SetRequestedParam(param);
	}

	const VideoCaptureParam& SinkFilter::GetResultingParam()
	{
		return m_sinkInputPin->GetResultingParam();
	}

	INT SinkFilter::GetPinCount()
	{
		return 1;
	}

	IPin* SinkFilter::GetPin(int index)
	{
		return index == 0 ? m_sinkInputPin : NULL;
	}

	STDMETHODIMP SinkFilter::GetClassID(CLSID* clsid)
	{
		*clsid = __uuidof(SinkFilter);
		return S_OK;
	}
}
