#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualList.h"
#include "TinyVisualScrollBar.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualListItem, TinyVisual);

		TinyVisualListItem::TinyVisualListItem()
		{

		}
		TinyVisualListItem::TinyVisualListItem(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualListItem::~TinyVisualListItem()
		{

		}

		TinyString TinyVisualListItem::RetrieveTag() const
		{
			return TinyVisualTagConst::LISTITEM;
		}

		void TinyVisualListItem::SetItemData(LPVOID data)
		{
			m_data = data;
		}

		LPVOID TinyVisualListItem::GetItemData() const
		{
			return m_data;
		}

		HRESULT TinyVisualListItem::OnMouseEnter()
		{
			TinyVisualList* ps = static_cast<TinyVisualList*>(m_spvisParent);
			if (ps != NULL)
			{
				m_document->SetFocus(ps->GetVScrollBar());
			}
			return TinyVisual::OnMouseEnter();
		}

		BOOL TinyVisualListItem::OnDraw(HDC hDC, const RECT& rcPaint)
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
			if (m_backgroundImage != NULL && !m_backgroundImage->IsEmpty())
			{
				if (!m_backgroundCenter.IsRectEmpty())
				{
					if (m_backgroundSize.IsEmpty())
					{
						canvas.DrawImage(*m_backgroundImage, clip, m_backgroundImage->GetRectangle(), m_backgroundCenter);
					}
					else
					{
						TinyRectangle rectangle;
						rectangle.SetRect(clip.Position(), m_backgroundSize);
						canvas.DrawImage(*m_backgroundImage, rectangle, m_backgroundImage->GetRectangle(), m_backgroundCenter);
					}
				}
				else
				{
					if (m_backgroundSize.IsEmpty())
					{
						canvas.DrawImage(*m_backgroundImage, clip, 0, 0, m_backgroundImage->GetSize().cx, m_backgroundImage->GetSize().cy);
					}
					else
					{
						TinyRectangle rectangle;
						rectangle.SetRect(clip.Position(), m_backgroundSize);
						canvas.DrawImage(*m_backgroundImage, rectangle, 0, 0, m_backgroundImage->GetSize().cx, m_backgroundImage->GetSize().cy);
					}
				}
			}
			if (!m_borderColor.IsEmpty() && m_borderThickness != -1)
			{
				TinyPen pen;
				pen.CreatePen(m_borderStyle, m_borderThickness, m_borderColor);
				canvas.SetBrush((HBRUSH)GetStockObject(NULL_BRUSH));
				canvas.SetPen(pen);
				canvas.DrawRectangle(clip);
			}
			canvas.SetFont(m_hFONT);
			canvas.SetTextColor(m_textColor);
			TinyRectangle rectangle;
			rectangle.SetRect(TinyPoint(clip.left, clip.bottom - 30), TinySize(120, 30));
			canvas.DrawString(m_szText, rectangle, m_textAlign);
			return TRUE;
		}

		//////////////////////////////////////////////////////////////////////////
		IMPLEMENT_DYNCREATE(TinyVisualList, TinyVisual);

		TinyVisualList::TinyVisualList()
			:m_count(5),
			m_iNewPos(0),
			m_pVScrollbar(NULL)
		{

		}
		TinyVisualList::TinyVisualList(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			: TinyVisual(spvisParent, vtree)
		{

		}

		HRESULT TinyVisualList::OnInitialize()
		{
			m_onPosChange.Reset(new Delegate<void(BOOL, INT, INT, INT)>(this, &TinyVisualList::OnPosChange));
			m_pVScrollbar = static_cast<TinyVisualVScrollBar*>(m_document->Create(TO_CX(m_rectangle) - 12, 0, 12, TO_CY(m_rectangle), TinyVisualTagConst::VSCROLLBAR, this));
			if (m_pVScrollbar != NULL)
			{
				m_pVScrollbar->EVENT_PosChange += m_onPosChange;
				m_pVScrollbar->SetVisible(FALSE);
			}
			return S_FALSE;
		}

		TinyVisualList::~TinyVisualList()
		{

		}

		TinyString TinyVisualList::RetrieveTag() const
		{
			return TinyVisualTagConst::LIST;
		}

		HRESULT	TinyVisualList::OnMouseEnter()
		{
			if (m_document != NULL)
			{
				m_document->SetFocus(m_pVScrollbar);
			}
			return TinyVisual::OnMouseEnter();
		}

		BOOL TinyVisualList::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			if (m_document != NULL)
			{
				TinyClipCanvas canvas(hDC, this, rcPaint);
				TinyRectangle clip = m_document->GetWindowRect(this);
				if (!m_backgroundColor.IsEmpty())
				{
					TinyBrush brush;
					brush.CreateBrush(m_backgroundColor);
					canvas.SetBrush(brush);
					canvas.FillRectangle(clip);
				}
				if (m_backgroundImage != NULL && !m_backgroundImage->IsEmpty())
				{
					if (!m_backgroundCenter.IsRectEmpty())
					{
						canvas.DrawImage(*m_backgroundImage, clip, m_backgroundImage->GetRectangle(), m_backgroundCenter);
					}
					else
					{
						canvas.DrawImage(*m_backgroundImage, clip, 0, 0, m_backgroundImage->GetSize().cx, m_backgroundImage->GetSize().cy);
					}
				}
				if (!m_borderColor.IsEmpty() && m_borderThickness != -1)
				{
					TinyPen pen;
					pen.CreatePen(m_borderStyle, m_borderThickness, m_borderColor);
					canvas.SetBrush((HBRUSH)GetStockObject(NULL_BRUSH));
					canvas.SetPen(pen);
					canvas.DrawRectangle(clip);
				}
			}
			return TRUE;
		}

		void TinyVisualList::OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos)
		{
			if (m_document != NULL)
			{
				AdjustLayout(0, iOldPos - iNewPos);
				m_iNewPos = iNewPos;
				m_document->Invalidate();
			}
		}

		TinyVisualVScrollBar* TinyVisualList::GetVScrollBar()
		{
			return m_pVScrollbar;
		}

		void TinyVisualList::AdjustLayout(INT dx, INT dy)
		{
			TinyVisual* spvis = m_document->GetVisual(this, CMD_CHILD);
			spvis = m_document->GetVisual(spvis, CMD_LAST);
			while (spvis != NULL)
			{
				if (spvis->IsKindOf(RUNTIME_CLASS(TinyVisualListItem)))
				{
					TinyRectangle s = spvis->GetRectangle();
					s.OffsetRect(dx, dy);
					spvis->SetPosition(s.Position());
					spvis->SetSize(s.Size());
				}
				spvis = m_document->GetVisual(spvis, CMD_PREV);
			}
		}

		void TinyVisualList::SetColumnCount(INT count)
		{
			m_count = count;
		}
		TinyVisualListItem* TinyVisualList::Add(const TinyString& text)
		{
			if (!m_document)
				return NULL;
			TinyVisualListItem* ps = static_cast<TinyVisualListItem*>(m_document->Create(TinyVisualTagConst::LISTITEM, this));
			if (ps != NULL)
			{
				INT row = (m_dwCount - 2) / m_count;//去掉滚动条
				INT column = (m_dwCount - 2) % m_count;//去掉滚动条
				INT itemCX = TO_CX(m_rectangle) / m_count - DEFAULT_LIST_ITEM_COLUMN_SPACE;//每行个数
				INT x = column * (itemCX + DEFAULT_LIST_ITEM_COLUMN_SPACE);
				INT y = row * (DEFAULT_LIST_ITEM_HEIGHT + DEFAULT_LIST_ITEM_COLUMN_SPACE);
				ps->SetPosition(TinyPoint(x, y));
				ps->SetSize(TinySize(itemCX, DEFAULT_LIST_ITEM_HEIGHT));
				ps->SetBackgroundSize(TinySize(itemCX, DEFAULT_LIST_ITEM_HEIGHT - 30));
				ps->SetText(text);
				ps->SetTextAlian(DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				ps->SetTextColor(RGB(64, 64, 64));
				y += (DEFAULT_LIST_ITEM_HEIGHT);
				if (y > TO_CY(m_rectangle))
				{
					m_pVScrollbar->SetVisible(TRUE);
					m_pVScrollbar->SetScrollInfo(0, y, TO_CY(m_rectangle), m_iNewPos, FALSE);
				}
				else
				{
					m_pVScrollbar->SetVisible(FALSE);
				}
				return ps;
			}
			return NULL;
		}
		TinyVisualListItem* TinyVisualList::Add(const TinyString& text, const TinyString& imageURL, const TinyString& defaultImage)
		{
			if (!m_document)
				return NULL;
			TinyVisualListItem* ps = static_cast<TinyVisualListItem*>(m_document->Create(TinyVisualTagConst::LISTITEM, this));
			if (ps != NULL)
			{
				TinyImage* image = new TinyImage();
				if (image && image->Open(imageURL.CSTR()))
				{
					ps->SetBackgroundImage(image);
				}
				else
				{
					ps->SetBackgroundImage(defaultImage);
				}
				INT row = (m_dwCount - 2) / m_count;//去掉滚动条
				INT column = (m_dwCount - 2) % m_count;//去掉滚动条
				INT itemCX = TO_CX(m_rectangle) / m_count - DEFAULT_LIST_ITEM_COLUMN_SPACE;//每行个数
				INT x = column * (itemCX + DEFAULT_LIST_ITEM_COLUMN_SPACE);
				INT y = row * (DEFAULT_LIST_ITEM_HEIGHT + DEFAULT_LIST_ITEM_COLUMN_SPACE);
				ps->SetPosition(TinyPoint(x, y));
				ps->SetSize(TinySize(itemCX, DEFAULT_LIST_ITEM_HEIGHT));
				ps->SetBackgroundSize(TinySize(120, DEFAULT_LIST_ITEM_HEIGHT - 30));
				ps->SetText(text);
				ps->SetTextAlian(DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				ps->SetTextColor(RGB(64, 64, 64));
				y += (DEFAULT_LIST_ITEM_HEIGHT);
				if (y > TO_CY(m_rectangle))
				{
					m_pVScrollbar->SetVisible(TRUE);
					m_pVScrollbar->SetScrollInfo(0, y, TO_CY(m_rectangle), m_iNewPos, FALSE);
				}
				else
				{
					m_pVScrollbar->SetVisible(FALSE);
				}
				return ps;
			}
			return NULL;
		}
		void TinyVisualList::RemoveAll(BOOL del)
		{
			if (m_document != NULL)
			{
				m_iNewPos = 0;
				if (m_pVScrollbar != NULL)
				{
					m_pVScrollbar->SetVisible(FALSE);
					m_pVScrollbar->SetScrollInfo(0, 0, 0, 0, FALSE);
				}
				TinyVisual* spvis = m_document->GetVisual(this, CMD_CHILD);
				spvis = m_document->GetVisual(spvis, CMD_LAST);
				while (spvis != NULL)
				{
					TinyVisual* spvisT = spvis;
					spvis = m_document->GetVisual(spvis, CMD_PREV);
					if (spvisT->IsKindOf(RUNTIME_CLASS(TinyVisualListItem)))
					{
						TinyVisualListItem* pItem = static_cast<TinyVisualListItem*>(spvisT);
						if (del)
						{
							LPVOID ps = pItem->GetItemData();
							SAFE_DELETE(ps);
						}
						TinyVisualResource::GetInstance().Remove(pItem->GetBackgroundImage());
						m_document->Destory(spvisT);
					}
				}
				Invalidate();
			}
		}
	}
}
