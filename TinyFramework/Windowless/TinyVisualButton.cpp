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
			switch (style)
			{
			case ButtonStyle::NORMAL:
				m_images[0] = image;
				break;
			case ButtonStyle::DOWN:
				m_images[1] = image;
				break;
			case ButtonStyle::HOVER:
				m_images[2] = image;
				break;
			}
		}
		BOOL TinyVisualButton::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyRectangle clip = m_document->GetWindowRect(this);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			canvas.SetFont(m_hFONT);
			canvas.SetTextColor(m_textColor);
			//»æÖÆ±³¾°
			TinyBrush brush;
			brush.CreateBrush(m_backgroundColor);
			canvas.SetBrush(brush);
			canvas.FillRectangle(clip);
			//»æÖÆÍ¼Æ¬
			TinyImage* image = NULL;
			switch (m_style)
			{
			case ButtonStyle::NORMAL:
				image = m_images[0];
				break;
			case ButtonStyle::DOWN:
				image = m_images[1];
				break;
			case ButtonStyle::HOVER:
				image = m_images[2];
				break;
			}
			if (image != NULL && !image->IsEmpty())
			{
				canvas.DrawImage(*image, clip, 0, 0, image->GetSize().cx, image->GetSize().cy);
			}
			//»æÖÆÎÄ×Ö
			if (!m_szText.IsEmpty())
			{
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
			if (m_style != ButtonStyle::NORMAL)
			{
				m_style = ButtonStyle::NORMAL;
				this->Invalidate();
			}
			BOOL bRes = TinyVisual::OnLButtonUp(pos, dwFlags);
			TinyRectangle s = m_document->GetWindowRect(this);
			TinyPoint point = m_document->VisualToClient(this, pos);
			if (s.PtInRect(point))
			{
				EVENT_CLICK(this, EventArgs());
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
