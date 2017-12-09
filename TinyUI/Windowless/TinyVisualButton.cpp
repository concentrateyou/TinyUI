#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualButton.h"
#include "TinyVisualFrame.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualButton, TinyVisual);

		TinyVisualButton::TinyVisualButton()
			:m_dwStyle(NORMAL)
		{

		}
		TinyVisualButton::TinyVisualButton(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			: TinyVisual(spvisParent, vtree),
			m_dwStyle(NORMAL)
		{
			ZeroMemory(&m_images, sizeof(m_images));
		}

		TinyVisualButton::~TinyVisualButton()
		{

		}
		TinyString TinyVisualButton::RetrieveTag() const
		{
			return TinyVisualTag::BUTTON;
		}
		void TinyVisualButton::SetStyleImage(StyleImage type, LPCSTR pzName)
		{
			m_images[(INT)type] = TinyVisualResource::GetInstance()[pzName];
		}
		BOOL TinyVisualButton::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualProperty::IMAGENORMAL.STR()) == 0)
			{
				this->SetStyleImage(NORMAL, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::IMAGEHIGHLIGHT.STR()) == 0)
			{
				this->SetStyleImage(HIGHLIGHT, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::IMAGEDOWN.STR()) == 0)
			{
				this->SetStyleImage(DOWN, value.STR());
				return TRUE;
			}
			return TinyVisual::SetProperty(name, value);
		}

		BOOL TinyVisualButton::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			if (!m_document)
				return FALSE;
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			canvas.SetFont(m_hFONT);
			canvas.SetTextColor(m_textColor);
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
			TinyImage* image = m_images[m_dwStyle];
			if (image != NULL && !image->IsEmpty())
			{
				canvas.DrawImage(*image, clip, 0, 0, image->GetSize().cx, image->GetSize().cy);
			}
			if (m_dwStyle == DOWN)
			{
				clip.OffsetRect(1, 1);
			}
			canvas.DrawString(GetText(), clip, m_textAlign);
			return TRUE;
		}

		HRESULT	TinyVisualButton::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			if (m_document != NULL)
			{
				m_document->SetCapture(this);
				if (m_dwStyle != DOWN)
				{
					m_dwStyle = DOWN;
					this->Invalidate();
				}
			}
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualButton::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			if (m_document != NULL)
			{
				StyleImage newStyle = dwFlags & MK_LBUTTON ? DOWN : HIGHLIGHT;
				if (newStyle != m_dwStyle)
				{
					m_dwStyle = newStyle;
					this->Invalidate();
				}
			}
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT TinyVisualButton::OnMouseLeave()
		{
			if (m_document != NULL)
			{
				if (m_dwStyle != NORMAL)
				{
					m_dwStyle = NORMAL;
					this->Invalidate();
				}
				m_document->ReleaseCapture();
			}
			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualButton::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			if (m_document != NULL)
			{
				if (m_dwStyle != NORMAL)
				{
					m_dwStyle = NORMAL;
					this->Invalidate();
				}
				m_document->SetCapture(NULL);
				TinyRectangle s = m_document->GetWindowRect(this);
				TinyPoint point = m_document->VisualToClient(this, pos);
				if (s.PtInRect(point))
				{
					EVENT_CLICK(this, EventArgs());
				}
			}
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
		HRESULT	TinyVisualButton::OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags)
		{
			if (m_document != NULL)
			{
				if (m_dwStyle != DOWN)
				{
					m_dwStyle = DOWN;
					this->Invalidate();
				}
				TinyRectangle s = m_document->GetWindowRect(this);
				TinyPoint point = m_document->VisualToClient(this, pos);
				if (s.PtInRect(point))
				{
					EVENT_CLICK(this, EventArgs());
				}
			}
			return TinyVisual::OnLButtonDBClick(pos, dwFlags);
		}
	}
}
