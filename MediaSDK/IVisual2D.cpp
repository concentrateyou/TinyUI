#include "stdafx.h"
#include "IVisual2D.h"

namespace MediaSDK
{
	IMPLEMENT_DYNAMIC(IVisual2D, TinyObject);

	IVisual2D::IVisual2D()
	{
	}

	IVisual2D::~IVisual2D()
	{
	}

	void IVisual2D::OnChangedRect(const TinyRectangle& rectOld)
	{
		if (m_trackerRect.left > 0)
		{
			XMFLOAT2 size = GetSize();
			SetScale(XMFLOAT2(static_cast<FLOAT>(TO_CX(m_trackerRect) / size.x), static_cast<FLOAT>(TO_CY(m_trackerRect) / size.y)));
			SetTranslate(XMFLOAT2(static_cast<FLOAT>(m_trackerRect.left), static_cast<FLOAT>(m_trackerRect.top)));
		}
	}
}