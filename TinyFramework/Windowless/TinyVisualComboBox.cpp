#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualComboBox.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualComboBox, TinyVisual);

		TinyVisualComboBox::TinyVisualComboBox()
			:m_style(ComboBoxStyle::NORMAL)
		{

		}
		TinyVisualComboBox::TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* document)
			: TinyVisual(spvisParent, document),
			m_style(ComboBoxStyle::NORMAL)
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
		HRESULT	 TinyVisualComboBox::OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			if (m_style != ComboBoxStyle::DOWN)
			{
				m_style = ComboBoxStyle::DOWN;
				this->Invalidate();
			}
			TinyRectangle s = m_document->GetWindowRect(this);
			TinyPoint point = m_document->VisualToClient(this, pos);
			if (s.PtInRect(point))
			{
				EVENT_CLICK(this, EventArgs());
			}
			return TinyVisual::OnLButtonDBClick(pos, dwFlags);

		}
		HRESULT	 TinyVisualComboBox::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			m_document->SetCapture(this);
			if (m_style != ComboBoxStyle::DOWN)
			{
				m_style = ComboBoxStyle::DOWN;
				this->Invalidate();
			}
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
				if (m_style != ComboBoxStyle::HOVER)
				{
					m_style = ComboBoxStyle::HOVER;
					this->Invalidate();
				}
				EVENT_CLICK(this, EventArgs());
			}
			else
			{
				if (m_style != ComboBoxStyle::NORMAL)
				{
					m_style = ComboBoxStyle::NORMAL;
					this->Invalidate();
				}
			}
			return bRes;
		}
		HRESULT	 TinyVisualComboBox::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			ComboBoxStyle style = dwFlags & MK_LBUTTON ? ComboBoxStyle::DOWN : ComboBoxStyle::HOVER;
			if (style != m_style)
			{
				m_style = style;
				this->Invalidate();
			}
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT	 TinyVisualComboBox::OnMouseLeave()
		{
			ASSERT(m_document);
			m_document->ReleaseCapture();
			if (m_style != ComboBoxStyle::NORMAL)
			{
				m_style = ComboBoxStyle::NORMAL;
				this->Invalidate();
			}
			return TinyVisual::OnMouseLeave();
		}
	}
}
