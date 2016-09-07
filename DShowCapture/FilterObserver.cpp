#include "stdafx.h"
#include "FilterObserver.h"

namespace Media
{
	FilterObserver::FilterObserver()
		:m_bits(NULL),
		m_size(0)
	{

	}
	FilterObserver::~FilterObserver()
	{

	}
	BYTE* FilterObserver::GetPointer() const
	{
		return m_bits;
	}
	INT	 FilterObserver::GetSize() const
	{
		return m_size;
	}
}
