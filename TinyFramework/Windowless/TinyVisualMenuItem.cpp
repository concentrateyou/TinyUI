#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualLabel.h"
#include "TinyVisualMenuItem.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualMenuItem, TinyVisual);

		TinyVisualMenuItem::TinyVisualMenuItem()
			:m_bChecked(FALSE),
			m_child(NULL)
		{
			m_bEnter = FALSE;
		}
		TinyVisualMenuItem::TinyVisualMenuItem(TinyVisual* spvisParent, TinyVisualDocument* document)
			: TinyVisual(spvisParent, document),
			m_bChecked(FALSE),
			m_child(NULL)
		{
			m_bEnter = FALSE;
		}
		TinyVisualMenuItem::~TinyVisualMenuItem()
		{

		}

		TinyString TinyVisualMenuItem::RetrieveTag() const
		{
			return TinyVisualTag::MENUITEM;
		}

		HRESULT	 TinyVisualMenuItem::OnMouseEnter()
		{
			ASSERT(m_document);
			m_bEnter = TRUE;
			this->Invalidate();
			return S_OK;
		}
		HRESULT	 TinyVisualMenuItem::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			return S_OK;
		}
		HRESULT	 TinyVisualMenuItem::OnMouseLeave()
		{
			ASSERT(m_document);
			m_bEnter = FALSE;
			this->Invalidate();
			return S_OK;
		}
		void TinyVisualMenuItem::SetImage(TinyImage* hl, TinyImage* checked)
		{
			m_images[0] = hl;
			m_images[1] = checked;
		}
		BOOL TinyVisualMenuItem::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyRectangle clip = m_document->GetWindowRect(this);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			if (m_bEnter && m_images[0] != NULL && !m_images[0]->IsEmpty())
			{
				canvas.DrawImage(*m_images[0], clip, 0, 0, m_images[0]->GetSize().cx, m_images[0]->GetSize().cy);
			}
			if (!m_szText.IsEmpty())
			{
				canvas.SetFont(m_hFONT);
				canvas.SetTextColor(m_bEnter ? RGB(255, 255, 255) : RGB(0, 0, 0));
				if (m_document->GetVisualHWND().RetrieveExStyle() & WS_EX_LAYERED)
				{
					TinyImage image;
					if (image.Create(hDC, m_szText.CSTR(), m_textAlign, m_bEnter ? RGB(0, 0, 0) : RGB(255, 255, 255)))
					{
						TinyRectangle s;
						s.SetSize(image.GetSize());
						s.SetPosition(TinyPoint(clip.left + 40, clip.top));
						canvas.DrawImage(image, s, 0, 0, image.GetSize().cx, image.GetSize().cy);
					}
				}
				else
				{
					SIZE size = TinyVisualDC::GetTextExtent(hDC, m_szText.CSTR(), m_szText.GetSize());
					clip.left += 40;
					clip.top += (clip.Height() - size.cy) / 2;
					clip.bottom -= (clip.Height() - size.cy) / 2;
					canvas.DrawString(m_szText, clip, m_textAlign);
				}
			}
			return TinyVisual::OnDraw(hDC, rcPaint);
		}
		void TinyVisualMenuItem::SetChecked(BOOL bChecked)
		{
			m_bChecked = bChecked;
		}
		BOOL TinyVisualMenuItem::IsChecked() const
		{
			return m_bChecked;
		}
	}
}
