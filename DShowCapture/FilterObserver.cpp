#include "stdafx.h"
#include "FilterObserver.h"

namespace DShow
{
	FilterObserver::FilterObserver()
		:m_lpParameter(NULL)
	{

	}
	FilterObserver::~FilterObserver()
	{

	}
	void FilterObserver::SetParameter(LPVOID lpParameter)
	{
		m_lpParameter = lpParameter;
	}
}
