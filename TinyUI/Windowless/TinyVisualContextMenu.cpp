#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualContextMenu.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualMenuItem, TinyVisual);

		TinyVisualMenuItem::TinyVisualMenuItem()
		{

		}
		TinyVisualMenuItem::TinyVisualMenuItem(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}

		TinyVisualMenuItem::~TinyVisualMenuItem()
		{

		}

		TinyUI::TinyString TinyVisualMenuItem::RetrieveTag() const
		{
			return TinyVisualTagConst::MENUITEM;
		}
		BOOL TinyVisualMenuItem::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			canvas.SetFont(m_hFONT);
			canvas.SetTextColor(m_textColor);
			canvas.DrawString(GetText(), clip, m_textAlign);
			return TRUE;
		}
		//////////////////////////////////////////////////////////////////////////
		IMPLEMENT_DYNCREATE(TinyVisualContextMenu, TinyVisual);

		TinyVisualContextMenu::TinyVisualContextMenu()
		{

		}
		TinyVisualContextMenu::TinyVisualContextMenu(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}

		TinyVisualContextMenu::~TinyVisualContextMenu()
		{

		}

		TinyUI::TinyString TinyVisualContextMenu::RetrieveTag() const
		{
			return TinyVisualTagConst::CONTEXTMENU;
		}

		BOOL TinyVisualContextMenu::Add(const TinyString& text, COLORREF color)
		{
			ASSERT(m_document);
			TinyVisualMenuItem* spvis = static_cast<TinyVisualMenuItem*>(m_document->Create(TinyVisualTagConst::MENUITEM, this));
			if (spvis != NULL)
			{
				TinyRectangle center = GetBackgroundCenter();
				INT x = center.left;
				INT y = center.top + m_items.GetSize() * DEFAULT_MENUITEM_HEIGHT;
				spvis->SetPosition(TinyPoint(x, y));
				spvis->SetSize(TinySize(DEFAULT_MENUITEM_WIDTH, DEFAULT_MENUITEM_HEIGHT));
				spvis->SetTextAlian(DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				spvis->SetTextColor(color);
				spvis->SetText(text);
				if (m_items.Add(spvis))
				{
					TinyRectangle imageRectangle = GetBackgroundImage()->GetRectangle();
					INT cx = center.left - imageRectangle.left + imageRectangle.right - center.right + DEFAULT_MENUITEM_WIDTH;
					INT cy = m_items.GetSize() * DEFAULT_MENUITEM_HEIGHT + center.top - imageRectangle.top + imageRectangle.bottom - center.bottom;
					this->SetSize(TinySize(cx, cy));
					m_document->GetVisualHWND().SetSize(TinySize(cx, cy));
					return TRUE;
				}
				return FALSE;
			}
			return FALSE;
		}
		BOOL TinyVisualContextMenu::Remove(TinyVisualMenuItem* spvis)
		{
			if (m_items.Remove(spvis))
			{
				TinyRectangle imageRect = GetBackgroundImage()->GetRectangle();
				TinyRectangle center = GetBackgroundCenter();
				INT cx = center.left - imageRect.left + imageRect.right - center.right + DEFAULT_MENUITEM_WIDTH;
				INT cy = m_items.GetSize() * DEFAULT_MENUITEM_HEIGHT + center.top - imageRect.top + imageRect.bottom - center.bottom;
				m_document->GetVisualHWND().SetSize(TinySize(cx, cy));
				return TRUE;
			}
			return FALSE;
		}
		void TinyVisualContextMenu::RemoveAll()
		{
			m_items.RemoveAll();
		}
		BOOL TinyVisualContextMenu::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			if (!m_backgroundColor.IsEmpty())
			{
				TinyBrush brush;
				brush.CreateBrush(m_backgroundColor);
				canvas.SetBrush(brush);
				canvas.FillRectangle(clip);
			}
			if (!m_borderColor.IsEmpty() && m_borderThickness != -1)
			{
				TinyPen pen;
				pen.CreatePen(m_borderStyle, m_borderThickness, m_borderColor);
				canvas.SetPen(pen);
				canvas.DrawRectangle(clip);
			}
			if (m_backgroundImage != NULL && !m_backgroundImage->IsEmpty())
			{
				TinyRectangle srcRect = m_backgroundImage->GetRectangle();
				TinyRectangle srcCenter = GetBackgroundCenter();
				if (srcCenter.IsRectEmpty())
				{
					canvas.DrawImage(*m_backgroundImage, clip, srcRect);
				}
				else
				{
					canvas.DrawImage(*m_backgroundImage, clip, srcRect, srcCenter);
				}
			}
			return TRUE;
		}
	}
}
