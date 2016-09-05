#include "stdafx.h"
#include "FilterObserver.h"

namespace Media
{
	FilterObserver::FilterObserver()
	{

	}
	FilterObserver::~FilterObserver()
	{

	}
	void FilterObserver::Lock()
	{
		m_lock.Acquire();
	}
	void FilterObserver::Unlock()
	{
		m_lock.Release();
	}
}
