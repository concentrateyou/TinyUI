#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualLabel.h"
#include "TinyVisualMenuItem.h"
#include "TinyVisualContextMenu.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualMenuItem, TinyVisual);

		TinyVisualMenuItem::TinyVisualMenuItem()
			:m_dwFlag(ITEM_NORMAL),
			m_child(NULL)
		{

		}
		TinyVisualMenuItem::TinyVisualMenuItem(TinyVisual* spvisParent, TinyVisualDocument* document)
			: TinyVisual(spvisParent, document),
			m_dwFlag(ITEM_NORMAL),
			m_child(NULL)
		{

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
			SetF(ITEM_HIGHLIGHT);
			Invalidate();
			return TinyVisual::OnMouseEnter();
		}
		HRESULT	 TinyVisualMenuItem::OnMouseLeave()
		{
			ASSERT(m_document);
			HRESULT hRes = TinyVisual::OnMouseLeave();
			ClrF(ITEM_HIGHLIGHT);
			Invalidate();
			return hRes;
		}
		HRESULT	TinyVisualMenuItem::OnMouseHover(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			if (m_child != NULL)
			{
				if (!m_child->IsPopup())
				{
					TinyPoint pos = m_document->GetScreenPos(this);
					pos.x += this->GetSize().cx;
					pos.y -= 6;
					m_child->Popup(pos);
				}
			}
			else
			{
				TinyVisualContextMenu* context = static_cast<TinyVisualContextMenu*>(&m_document->GetVisualHWND());
				for (INT i = 0; i < context->m_contexts.GetSize(); i++)
				{
					TinyVisualContextMenu* s = context->m_contexts[i];
					s->Unpopup();
				}
			}
			return TinyVisual::OnMouseHover(pos, dwFlags);
		}
		HRESULT	 TinyVisualMenuItem::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			TinyRectangle s = m_document->GetWindowRect(this);
			TinyPoint point = m_document->VisualToClient(this, pos);
			HRESULT hRes = TinyVisual::OnLButtonUp(pos, dwFlags);
			if (s.PtInRect(point))
			{
				if (!TestF(ITEM_CHILD))
				{
					EVENT_CLICK(this, EventArgs());
				}
			}
			return hRes;
		}
		void TinyVisualMenuItem::SetImageList(TinyImage* icon,
			TinyImage* highlight,
			TinyImage* check,
			TinyImage* arrow)
		{
			m_images[0] = highlight;
			m_images[1] = check;
			m_images[2] = arrow;
			m_images[3] = icon;
		}
		TinyVisualMenuItem* TinyVisualMenuItem::Add()
		{
			if (!m_child)
			{
				TinyVisualContextMenu* context = static_cast<TinyVisualContextMenu*>(&m_document->GetVisualHWND());
				m_child = new TinyVisualContextMenu();
				m_child->Create(context->m_hWND, "");
				context->m_contexts.Add(m_child);
				m_child->m_owner = context;
			}
			TinyVisualMenuItem* spvis = m_child->Add();
			ASSERT(spvis);
			UINT count = m_child->GetDocument().GetParent(NULL)->GetChildCount();
			if (count > 0)
				SetF(ITEM_CHILD);
			else
				ClrF(ITEM_CHILD);
			return spvis;
		}
		TinyVisualMenuItem* TinyVisualMenuItem::Add(const TinyString& name, const TinyString& text, TinyImage* icon)
		{
			if (!m_child)
			{
				TinyVisualContextMenu* context = static_cast<TinyVisualContextMenu*>(&m_document->GetVisualHWND());
				m_child = new TinyVisualContextMenu();
				m_child->Create(context->Handle(), "");
				context->m_contexts.Add(m_child);
				m_child->m_owner = context;
			}
			TinyVisualMenuItem* spvis = m_child->Add(name, text, icon);
			ASSERT(spvis);
			UINT count = m_child->GetDocument().GetParent(NULL)->GetChildCount();
			if (count > 0)
				SetF(ITEM_CHILD);
			else
				ClrF(ITEM_CHILD);
			return spvis;
		}
		void TinyVisualMenuItem::Remove(const TinyString& name)
		{
			if (m_child != NULL)
			{
				m_child->Remove(name);
				UINT count = m_child->GetDocument().GetParent(NULL)->GetChildCount();
				if (count > 0)
					SetF(ITEM_CHILD);
				else
					ClrF(ITEM_CHILD);
			}
		}
		void TinyVisualMenuItem::RemoveAll()
		{
			if (m_child != NULL)
			{
				m_child->DestroyWindow();
				m_child = NULL;
				ClrF(ITEM_CHILD);
			}
		}
		BOOL TinyVisualMenuItem::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyRectangle clip = m_document->GetWindowRect(this);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			if (TestF(ITEM_SEPARATOR))
			{
				if (m_backgroundImage != NULL && !m_backgroundImage->IsEmpty())
				{
					clip.left += m_padding.left;
					canvas.DrawImage(*m_backgroundImage, clip, m_backgroundRectangle);
				}
			}
			else
			{
				if (m_backgroundImage != NULL)
				{
					if (m_backgroundCenter.IsRectEmpty())
					{
						canvas.DrawImage(*m_backgroundImage, clip, m_backgroundRectangle);
					}
					else
					{
						canvas.DrawImage(*m_backgroundImage, clip, m_backgroundRectangle, m_backgroundCenter);
					}
				}
				if (TestF(ITEM_HIGHLIGHT))
				{
					if (m_images[0] != NULL && !m_images[0]->IsEmpty())
					{
						TinyRectangle s = clip;
						s.DeflateRect(1, 1, 1, 1);
						canvas.DrawImage(*m_images[0], s, 0, 0, m_images[0]->GetSize().cx, m_images[0]->GetSize().cy);
					}
				}
				if (TestF(ITEM_CHECKED))
				{
					if (m_images[1] != NULL && !m_images[1]->IsEmpty())
					{
						TinyRectangle s = clip;
						s.left += (m_padding.left - m_images[1]->GetSize().cx) / 2;
						s.top += (clip.Height() - m_images[1]->GetSize().cy) / 2;
						s.SetSize(m_images[1]->GetSize());
						canvas.DrawImage(*m_images[1], s, 0, 0, m_images[1]->GetSize().cx, m_images[1]->GetSize().cy);
					}
				}
				if (TestF(ITEM_CHILD))
				{
					if (m_images[2] != NULL && !m_images[2]->IsEmpty())
					{
						TinyRectangle s = clip;
						s.right -= (m_images[2]->GetSize().cx) / 2 + 5;
						s.top += (clip.Height() - m_images[2]->GetSize().cy) / 2;
						s.left = s.right - m_images[2]->GetSize().cx;
						s.bottom = s.top + m_images[2]->GetSize().cy;
						canvas.DrawImage(*m_images[2], s, 0, 0, m_images[2]->GetSize().cx, m_images[2]->GetSize().cy);
					}
				}
				if (!m_szText.IsEmpty())
				{
					canvas.SetFont(m_hFONT);
					canvas.SetTextColor(TestF(ITEM_HIGHLIGHT) ? RGB(255, 255, 255) : RGB(0, 0, 0));
					if (m_document->GetVisualHWND().RetrieveExStyle() & WS_EX_LAYERED)
					{
						TinyImage textImage;
						if (textImage.Create(hDC, m_szText.CSTR(), m_textAlign, TestF(ITEM_HIGHLIGHT) ? RGB(0, 0, 0) : RGB(255, 255, 255)))
						{
							TinyRectangle s;
							s.SetSize(textImage.GetSize());
							s.SetPosition(TinyPoint(clip.left + m_padding.left, clip.top + (clip.Height() - textImage.GetSize().cy) / 2));
							canvas.DrawImage(textImage, s, 0, 0, textImage.GetSize().cx, textImage.GetSize().cy);
						}
					}
					else
					{
						SIZE size = TinyVisualDC::GetTextExtent(hDC, m_szText.CSTR(), m_szText.GetSize());
						clip.left += m_padding.left;
						clip.top += (clip.Height() - size.cy) / 2;
						clip.bottom -= (clip.Height() - size.cy) / 2;
						canvas.DrawString(m_szText, clip, m_textAlign);
					}
				}
			}
			return TinyVisual::OnDraw(hDC, rcPaint);
		}
		TinyVisualContextMenu*	TinyVisualMenuItem::GetContextMenu()
		{
			return m_child;
		}
		void TinyVisualMenuItem::SetSeparator(BOOL bSeparator)
		{
			if (bSeparator)
				SetF(ITEM_SEPARATOR);
			else
				ClrF(ITEM_SEPARATOR);
		}
		BOOL TinyVisualMenuItem::IsSeparator() const
		{
			return TestF(ITEM_SEPARATOR);
		}
		void TinyVisualMenuItem::SetChecked(BOOL bChecked)
		{
			if (bChecked)
				SetF(ITEM_CHECKED);
			else
				ClrF(ITEM_CHECKED);
			Invalidate();
		}
		BOOL TinyVisualMenuItem::IsChecked() const
		{
			return TestF(ITEM_CHECKED);
		}
	}
}
