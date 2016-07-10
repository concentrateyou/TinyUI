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
			return TinyVisualTag::BUTTON;
		}
		BOOL TinyVisualButton::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyImage& image = m_images[m_dwFlag];
			if (image.IsEmpty())
				return FALSE;
			TinyClipCanvas canvas(hDC, this, rcPaint);
			if (canvas.IsValid())
			{
				TinyRectangle clip = canvas.GetClipBox();
				canvas.DrawImage(image, clip, 0, 0, image.GetSize().cx, image.GetSize().cy);
			}
			return TRUE;
		}

		HRESULT	TinyVisualButton::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = DOWN;
			TinyRectangle s = m_vtree->GetWindowRect(this);
			m_vtree->Redraw(&s);
			m_vtree->SetCapture(this);
			RECT rect = m_rectangle;
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualButton::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = dwFlags & MK_LBUTTON ? DOWN : HIGHLIGHT;
			TinyRectangle s = m_vtree->GetWindowRect(this);
			m_vtree->Redraw(&s);
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT TinyVisualButton::OnMouseLeave()
		{
			m_dwFlag = NORMAL;
			TinyRectangle s = m_vtree->GetWindowRect(this);
			m_vtree->Redraw(&s);
			m_vtree->ReleaseCapture();
			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualButton::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = HIGHLIGHT;
			TinyRectangle s = m_vtree->GetWindowRect(this);
			m_vtree->Redraw(&s);
			m_vtree->SetCapture(NULL);
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
	}
}
