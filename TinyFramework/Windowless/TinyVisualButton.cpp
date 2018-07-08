#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualButton.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualButton, TinyVisual);

		TinyVisualButton::TinyVisualButton()
			:m_style(ButtonStyle::NORMAL)
		{

		}
		TinyVisualButton::TinyVisualButton(TinyVisual* spvisParent, TinyVisualDocument* document)
			: TinyVisual(spvisParent, document),
			m_style(ButtonStyle::NORMAL)
		{

		}
		TinyVisualButton::~TinyVisualButton()
		{

		}

		TinyString TinyVisualButton::RetrieveTag() const
		{
			return TinyVisualTag::BUTTON;
		}
		void	TinyVisualButton::SetImage(ButtonStyle style, TinyImage* image)
		{
			m_images[static_cast<BYTE>(style)] = image;
			Invalidate();
		}
		BOOL TinyVisualButton::OnDraw(HDC hDC, const RECT& rcPaint)
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
			if (!m_szText.IsEmpty())
			{
				clip.DeflateRect(m_padding);
				if (m_style == ButtonStyle::DOWN)
				{
					clip.OffsetRect(1, 1);
				}
				canvas.DrawString(m_szText, clip, m_textAlign);
			}
			return FALSE;
		}
		HRESULT	 TinyVisualButton::OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			if (m_style != ButtonStyle::DOWN)
			{
				m_style = ButtonStyle::DOWN;
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
		HRESULT	 TinyVisualButton::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			m_document->SetCapture(this);
			if (m_style != ButtonStyle::DOWN)
			{
				m_style = ButtonStyle::DOWN;
				this->Invalidate();
			}
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	 TinyVisualButton::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			m_document->SetCapture(NULL);
			BOOL bRes = TinyVisual::OnLButtonUp(pos, dwFlags);
			TinyRectangle s = m_document->GetWindowRect(this);
			TinyPoint point = m_document->VisualToClient(this, pos);
			if (s.PtInRect(point))
			{
				if (m_style != ButtonStyle::HOVER)
				{
					m_style = ButtonStyle::HOVER;
					this->Invalidate();
				}
				EVENT_CLICK(this, EventArgs());
			}
			else
			{
				if (m_style != ButtonStyle::NORMAL)
				{
					m_style = ButtonStyle::NORMAL;
					this->Invalidate();
				}
			}
			return bRes;
		}
		HRESULT	 TinyVisualButton::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_document);
			ButtonStyle style = dwFlags & MK_LBUTTON ? ButtonStyle::DOWN : ButtonStyle::HOVER;
			if (style != m_style)
			{
				m_style = style;
				this->Invalidate();
			}
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT	 TinyVisualButton::OnMouseLeave()
		{
			ASSERT(m_document);
			m_document->ReleaseCapture();
			if (m_style != ButtonStyle::NORMAL)
			{
				m_style = ButtonStyle::NORMAL;
				this->Invalidate();
			}
			return TinyVisual::OnMouseLeave();
		}
	}
}
