#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualComboBox.h"
#include "TinyVisualDropDownHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualComboBox, TinyVisual);
		TinyVisualComboBox::TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree),
			m_dwFlag(NORMAL),
			m_dwArrawFlag(NORMAL),
			m_bActive(FALSE)
		{
			m_onPopupActive.Reset(new Delegate<void(ActiveEventArgs&)>(this, &TinyVisualComboBox::OnPopupActive));
			m_popup.EVENT_ACTIVE += m_onPopupActive;
		}
		TinyVisualComboBox::~TinyVisualComboBox()
		{
			m_popup.EVENT_ACTIVE -= m_onPopupActive;
		}
		TinyString TinyVisualComboBox::RetrieveTag() const
		{
			return TinyVisualTag::COMBOBOX;
		}
		BOOL TinyVisualComboBox::SetStyleImage(StyleImage type, LPCSTR pzFile)
		{
			if (!m_images[(INT)type].Open(pzFile))
				return FALSE;
			return TRUE;
		}
		BOOL TinyVisualComboBox::SetArrowImage(StyleImage type, LPCSTR pzFile)
		{
			if (!m_arraws[(INT)type].Open(pzFile))
				return FALSE;
			return TRUE;
		}
		BOOL TinyVisualComboBox::SetProperty(const TinyString& name, const TinyString& value)
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
			if (strcasecmp(name.STR(), TinyVisualProperty::IMAGEPUSH.STR()) == 0)
			{
				this->SetStyleImage(PUSH, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::IMAGEDOWN.STR()) == 0)
			{
				this->SetStyleImage(DOWN, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROWNORMAL.STR()) == 0)
			{
				this->SetArrowImage(NORMAL, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROWHIGHLIGHT.STR()) == 0)
			{
				this->SetArrowImage(HIGHLIGHT, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROWDOWN.STR()) == 0)
			{
				this->SetArrowImage(DOWN, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::ARROWPUSH.STR()) == 0)
			{
				this->SetArrowImage(PUSH, value.STR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::DROPDOWN.STR()) == 0)
			{
				m_popup.BuildResource(value.STR());
				return TRUE;
			}
			return TinyVisual::SetProperty(name, value);
		}
		BOOL TinyVisualComboBox::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyImage& image = m_images[m_dwFlag];
			if (image.IsEmpty())
				return FALSE;
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			canvas.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));
			canvas.SetTextColor(m_textColor);
			TinyRectangle srcPaint = image.GetRectangle();
			TinyRectangle srcCenter(srcPaint.left + 5, srcPaint.top + 5, srcPaint.right - 5, srcPaint.bottom - 5);
			canvas.DrawImage(image, clip, srcPaint, srcCenter);
			TinyImage& arrow = m_arraws[m_dwArrawFlag];
			srcPaint = arrow.GetRectangle();
			TinyRectangle dstPaint(TinyPoint(clip.right - srcPaint.Width() - 3, clip.top + (clip.Height() - srcPaint.Height()) / 2), srcPaint.Size());
			canvas.DrawImage(arrow, dstPaint, srcPaint);
			clip.OffsetRect(7, 0);
			canvas.DrawString(GetText(), clip, m_textAlign);
			return TRUE;
		}
		HRESULT	TinyVisualComboBox::OnCreate()
		{
			m_popup.Create(m_document->GetVisualHWND()->Handle(), NULL);
			m_popup.UpdateWindow();
			return TinyVisual::OnCreate();
		}
		HRESULT TinyVisualComboBox::OnDestory()
		{
			m_popup.DestroyWindow();
			return TinyVisual::OnDestory();
		}

		HRESULT	TinyVisualComboBox::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = PUSH;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Redraw(&s);
			m_document->ReleaseCapture();
			TinyPoint screenPos = m_document->GetScreenPos(this);
			screenPos.Offset(1, s.Height());
			TinySize size = s.Size();
			size.cx = size.cx - 2;
			m_popup.AllowTracking(FALSE);
			m_popup.SetPosition(screenPos, size);
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualComboBox::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = m_bActive ? PUSH : (dwFlags & MK_LBUTTON ? DOWN : HIGHLIGHT);
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Redraw(&s);
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT TinyVisualComboBox::OnMouseLeave()
		{
			m_dwFlag = m_bActive ? PUSH : NORMAL;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Redraw(&s);
			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualComboBox::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = PUSH;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Redraw(&s);
			EVENT_CLICK(EventArgs());
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
		void TinyVisualComboBox::OnPopupActive(ActiveEventArgs& args)
		{
			m_bActive = args.IsActive();
			m_dwArrawFlag = m_bActive ? DOWN : NORMAL;
			if (!m_bActive)
			{
				ShowWindow(m_popup, SW_HIDE);
				TinyRectangle s = m_document->GetWindowRect(this);
				m_document->Redraw(&s);
			}
		}
	}
}
