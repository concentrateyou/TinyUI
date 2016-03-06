#include "../stdafx.h"
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisual::TinyVisual()
			:m_spvisPrev(NULL),
			m_spvisNext(NULL),
			m_spvisParent(NULL),
			m_spvisChild(NULL),
			m_spvisOwner(NULL)
		{

		}
		TinyVisual::~TinyVisual()
		{

		}
		void TinyVisual::SetMaximumSize(const TinySize& size)
		{
			if (m_maximumSize != size)
			{
				m_maximumSize = size;
			}
		}
		void TinyVisual::SetMinimumSize(const TinySize& size)
		{
			if (m_minimumSize != size)
			{
				m_minimumSize = size;
			}
		}
		TinySize TinyVisual::GetMaximumSize() const
		{
			return m_maximumSize;
		}
		TinySize TinyVisual::GetMinimumSize() const
		{
			return m_minimumSize;
		}
		HRESULT	TinyVisual::OnDraw(TinyDC& dc, TinyRectangle& drawRect)
		{
			return FALSE;
		}
	}
}
