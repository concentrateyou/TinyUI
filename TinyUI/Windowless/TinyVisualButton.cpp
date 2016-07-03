#include "../stdafx.h"
#include "TinyVisualTree.h"
#include "TinyVisualManage.h"
#include "TinyVisualButton.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualButton::TinyVisualButton(TinyVisual* spvisParent, TinyVisualTree* vtree)
			:TinyVisual(spvisParent, vtree),
			m_dwFlag(NORMAL)
		{

		}
		TinyVisualButton::~TinyVisualButton()
		{

		}
		TinyString TinyVisualButton::RetrieveTag() const
		{
			return TinyVisualTag::SYSBUTTON;
		}
		BOOL TinyVisualButton::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TRACE("m_dwFlag:%d\n", m_dwFlag);
			TinyImage& image = m_images[m_dwFlag];
			if (image.IsEmpty())
				return FALSE;
			TinyClipCanvas canvas(hDC, this);
			TinyRectangle clip = canvas.GetClipBox();
			canvas.DrawImage(image, clip, 0, 0, image.GetSize().cx, image.GetSize().cy);
			return TRUE;
		}

		HRESULT	TinyVisualButton::OnLButtonDown(TinyPoint pos, DWORD dwFlags)
		{
			m_dwFlag = DOWN;
			m_vtree->Invalidate();
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualButton::OnMouseMove(TinyPoint pos, DWORD dwFlags)
		{
			m_dwFlag = HIGHLIGHT;
			m_vtree->Invalidate();
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT TinyVisualButton::OnMouseLeave()
		{
			m_dwFlag = NORMAL;
			m_vtree->Invalidate();
			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualButton::OnLButtonUp(TinyPoint pos, DWORD dwFlags)
		{
			m_dwFlag = NORMAL;
			m_vtree->Invalidate();
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
	}
}
