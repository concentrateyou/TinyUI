#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualComboBoxItem.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualComboBoxItem, TinyVisual);

		TinyVisualComboBoxItem::TinyVisualComboBoxItem()
			:m_dwFlag(NULL)
		{

		}
		TinyVisualComboBoxItem::TinyVisualComboBoxItem(TinyVisual* spvisParent, TinyVisualDocument* document)
			: TinyVisual(spvisParent, document)
		{

		}
		TinyVisualComboBoxItem::~TinyVisualComboBoxItem()
		{

		}

		TinyString TinyVisualComboBoxItem::RetrieveTag() const
		{
			return TinyVisualTag::COMBOBOXITEM;
		}
		HRESULT	 TinyVisualComboBoxItem::OnMouseEnter()
		{
			ASSERT(m_document);
			SetF(ITEM_HIGHLIGHT);
			Invalidate();
			return TinyVisual::OnMouseEnter();
		}
		HRESULT	 TinyVisualComboBoxItem::OnMouseHover(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			SetF(ITEM_HIGHLIGHT);
			Invalidate();
			return TinyVisual::OnMouseHover(pos, dwFlags);
		}
		HRESULT	 TinyVisualComboBoxItem::OnMouseLeave()
		{
			ASSERT(m_document);
			ClrF(ITEM_HIGHLIGHT);
			Invalidate();
			return TinyVisual::OnMouseLeave();
		}
		HRESULT	 TinyVisualComboBoxItem::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			TinyRectangle s = m_document->GetWindowRect(this);
			TinyPoint point = m_document->VisualToClient(this, pos);
			HRESULT hRes = TinyVisual::OnLButtonUp(pos, dwFlags);
			if (s.PtInRect(point))
			{
				EVENT_CLICK(this, EventArgs());
			}
			return hRes;
		}
		BOOL TinyVisualComboBoxItem::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyRectangle clip = m_document->GetWindowRect(this);
			TinyClipCanvas canvas(hDC, this, rcPaint);
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
					TinyRectangle s = rcPaint;
					s.DeflateRect(1, 1, 1, 1);
					TinyClipCanvas clipCanvas(hDC, this, s);
					clipCanvas.DrawImage(*m_images[0], clip, 0, 0, m_images[0]->GetSize().cx, m_images[0]->GetSize().cy);
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
						TinyRectangle s1 = rcPaint;
						s1.DeflateRect(3, 3, 3, 3);
						TinyClipCanvas clipCanvas(hDC, this, s1);
						clipCanvas.DrawImage(textImage, s, 0, 0, textImage.GetSize().cx, textImage.GetSize().cy);
					}
				}
				else
				{
					SIZE size = TinyVisualDC::GetTextExtent(hDC, m_szText.CSTR(), m_szText.GetSize());
					clip.left += m_padding.left;
					clip.top += (clip.Height() - size.cy) / 2;
					clip.bottom -= (clip.Height() - size.cy) / 2;
					TinyRectangle s = rcPaint;
					s.DeflateRect(3, 3, 3, 3);
					TinyClipCanvas clipCanvas(hDC, this, s);
					clipCanvas.DrawString(m_szText, clip, m_textAlign);
				}
			}
			return TinyVisual::OnDraw(hDC, rcPaint);
		}
		void TinyVisualComboBoxItem::SetImage(TinyImage* highlight)
		{
			m_images[0] = highlight;
		}
	}
}
