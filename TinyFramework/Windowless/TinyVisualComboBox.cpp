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
			m_comboboxHWND(NULL)
		{

		}
		TinyVisualComboBox::TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* document)
			: TinyVisual(spvisParent, document),
			m_style(ComboBoxStyle::NORMAL),
			m_comboboxHWND(NULL)
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
		}
		TinyVisualComboBoxItem*	TinyVisualComboBox::Add(const TinyString& name, const TinyString& text)
		{
			ASSERT(m_document);
			if (!m_comboboxHWND)
			{
				m_comboboxHWND = new TinyVisualComboBoxHWND();
				m_comboboxHWND->Create(m_document->GetVisualHWND(), "");
				m_comboboxHWND->m_owner = this;
			}
			return m_comboboxHWND->Add(name, text);
		}
		void TinyVisualComboBox::Remove(const TinyString& name)
		{
			if (m_comboboxHWND != NULL)
			{
				m_comboboxHWND->Remove(name);
			}
		}
		void TinyVisualComboBox::RemoveAll()
		{
			if (m_comboboxHWND != NULL)
			{
				m_comboboxHWND->RemoveAll();
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
		HRESULT	TinyVisualComboBox::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			m_document->SetCapture(this);
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	 TinyVisualComboBox::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			m_document->SetCapture(NULL);
			BOOL bRes = TinyVisual::OnLButtonUp(pos, dwFlags);
			TinyRectangle s = m_document->GetWindowRect(this);
			TinyPoint point = m_document->VisualToClient(this, pos);
			if (s.PtInRect(point))
			{
				if (m_comboboxHWND != NULL)
				{
					TinyPoint pos = m_document->GetScreenPos(this);
					pos.y += GetSize().cy;
					m_comboboxHWND->Popup(pos);
					m_style = ComboBoxStyle::PUSH;
					this->Invalidate();
				}
			}
			return bRes;
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
