#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualComboBoxItem.h"
#include "TinyVisualComboBoxHWND.h"
#include "TinyVisualComboBox.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualComboBox, TinyVisual);

		TinyVisualComboBox::TinyVisualComboBox()
			:m_style(ComboBoxStyle::NORMAL),
			m_combobox(NULL),
			m_itemsize(125, 26)
		{

		}
		TinyVisualComboBox::TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* document)
			: TinyVisual(spvisParent, document),
			m_style(ComboBoxStyle::NORMAL),
			m_combobox(NULL),
			m_itemsize(125, 26)
		{

		}
		TinyVisualComboBox::~TinyVisualComboBox()
		{

		}

		TinyString TinyVisualComboBox::RetrieveTag() const
		{
			return TinyVisualTag::COMBOBOX;
		}
		void TinyVisualComboBox::SetImage(ComboBoxStyle style, TinyImage* image)
		{
			m_images[static_cast<BYTE>(style)] = image;
			Invalidate();
		}
		void TinyVisualComboBox::SetItemSize(const TinySize& itemsize)
		{
			m_itemsize = itemsize;
			if (m_combobox != NULL)
			{
				m_combobox->SetItemSize(itemsize);
			}
		}
		void TinyVisualComboBox::SetSelected(TinyVisualComboBoxItem* item)
		{
			if (m_combobox != NULL)
			{
				m_combobox->m_current = item;
				m_combobox->Invalidate();
				SetText(item != NULL ? item->GetText() : "");
				Invalidate();
			}
		}
		TinySize TinyVisualComboBox::GetItemSize() const
		{
			return m_itemsize;
		}
		TinyVisualComboBoxItem*	TinyVisualComboBox::GetSelected()
		{
			if (m_combobox != NULL)
				return static_cast<TinyVisualComboBoxItem*>(m_combobox->m_current);
			return NULL;
		}
		TinyVisualComboBoxItem*	TinyVisualComboBox::Add(const TinyString& name, const TinyString& text)
		{
			ASSERT(m_document);
			if (!m_combobox)
			{
				m_combobox = new TinyVisualComboBoxHWND();
				m_combobox->Create(m_document->GetVisualHWND(), "");
				m_combobox->m_owner = this;
			}
			return m_combobox->Add(name, text);
		}
		void TinyVisualComboBox::Remove(const TinyString& name)
		{
			if (m_combobox != NULL)
			{
				m_combobox->Remove(name);
			}
		}
		void TinyVisualComboBox::RemoveAll()
		{
			if (m_combobox != NULL)
			{
				m_combobox->RemoveAll();
			}
		}
		BOOL TinyVisualComboBox::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyRectangle clip = m_document->GetWindowRect(this);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			canvas.SetFont(m_hFONT);
			canvas.SetTextColor(m_textColor);

			TinyBrush brush;
			brush.CreateBrush(m_backgroundColor);
			canvas.SetBrush(brush);
			canvas.FillRectangle(clip);

			TinyImage* image = m_images[static_cast<BYTE>(m_style)];
			if (image != NULL && !image->IsEmpty())
			{
				TinyRectangle center = image->GetCenter();
				if (center.IsRectEmpty())
					canvas.DrawImage(*image, clip, 0, 0, image->GetSize().cx, image->GetSize().cy);
				else
					canvas.DrawImage(*image, clip, { 0, 0, image->GetSize().cx, image->GetSize().cy }, center);
			}
			image = m_images[static_cast<BYTE>(m_style) + 3];
			if (image != NULL && !image->IsEmpty())
			{
				TinyRectangle s = clip;
				s.right -= (image->GetSize().cx) / 2;
				s.top += (clip.Height() - image->GetSize().cy) / 2;
				s.left = s.right - image->GetSize().cx;
				s.bottom = s.top + image->GetSize().cy;
				TinyRectangle center = image->GetCenter();
				if (center.IsRectEmpty())
					canvas.DrawImage(*image, s, 0, 0, image->GetSize().cx, image->GetSize().cy);
				else
					canvas.DrawImage(*image, s, { 0, 0, image->GetSize().cx, image->GetSize().cy }, center);
			}
			if (!m_szText.IsEmpty())
			{
				clip.DeflateRect(m_padding);
				canvas.DrawString(m_szText, clip, m_textAlign);
			}
			return FALSE;
		}
		HRESULT	TinyVisualComboBox::OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			m_document->SetCapture(this);
			BOOL bRes = TinyVisual::OnLButtonDBClick(pos, dwFlags);
			TinyRectangle s = m_document->GetWindowRect(this);
			TinyPoint point = m_document->VisualToClient(this, pos);
			if (s.PtInRect(point))
			{
				if (m_combobox != NULL)
				{
					if (m_combobox->IsPopup())
					{
						m_combobox->Unpopup();
						m_style = ComboBoxStyle::NORMAL;
						this->Invalidate();
					}
					else
					{
						TinyPoint pos = m_document->GetScreenPos(this);
						pos.y += GetSize().cy;
						if (m_combobox->m_current != NULL)
						{
							TinyVisualComboBoxItem* current = static_cast<TinyVisualComboBoxItem*>(m_combobox->m_current);
							current->m_dwFlag |= ITEM_HIGHLIGHT;
						}
						m_combobox->Popup(pos, 0);
						m_style = ComboBoxStyle::PUSH;
						this->Invalidate();
					}
				}
			}
			return bRes;
		}
		HRESULT	TinyVisualComboBox::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			m_document->SetCapture(this);
			BOOL bRes = TinyVisual::OnLButtonDown(pos, dwFlags);
			TinyRectangle s = m_document->GetWindowRect(this);
			TinyPoint point = m_document->VisualToClient(this, pos);
			if (s.PtInRect(point))
			{
				if (m_combobox != NULL)
				{
					if (m_combobox->IsPopup())
					{
						m_combobox->Unpopup();
						m_style = ComboBoxStyle::NORMAL;
						this->Invalidate();
					}
					else
					{
						TinyPoint pos = m_document->GetScreenPos(this);
						pos.y += GetSize().cy;
						if (m_combobox->m_current != NULL)
						{
							TinyVisualComboBoxItem* current = static_cast<TinyVisualComboBoxItem*>(m_combobox->m_current);
							current->m_dwFlag |= ITEM_HIGHLIGHT;
						}
						m_combobox->Popup(pos, 0);
						m_style = ComboBoxStyle::PUSH;
						this->Invalidate();
					}
				}
			}
			return bRes;
		}
		HRESULT	 TinyVisualComboBox::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			m_document->SetCapture(NULL);
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
		HRESULT	 TinyVisualComboBox::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT	 TinyVisualComboBox::OnMouseLeave()
		{
			ASSERT(m_document);
			m_document->ReleaseCapture();
			return TinyVisual::OnMouseLeave();
		}
	}
}
