#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualComboBox.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualComboBox::TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree),
			m_dwFlag(NORMAL)
		{

		}
		TinyVisualComboBox::~TinyVisualComboBox()
		{

		}
		TinyString TinyVisualComboBox::RetrieveTag() const
		{
			return TinyVisualTag::COMBOBOX;
		}

		BOOL TinyVisualComboBox::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyImage& image = m_images[m_dwFlag];
			if (image.IsEmpty())
				return FALSE;
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			canvas.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));
			canvas.SetTextColor(RGB(255, 255, 255));
			RECT srcPaint = image.GetRectangle();
			RECT srcCenter = { srcPaint.left + 4, srcPaint.top + 4, srcPaint.right - 4, srcPaint.bottom - 4 };
			canvas.DrawImage(image, clip, srcPaint, srcCenter);
			canvas.DrawString(GetText(), clip, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			return TRUE;
		}
		HRESULT	TinyVisualComboBox::OnCreate()
		{
			TinySize size = this->GetSize();
			SetStyleImage(DOWN, "D:\\Develop\\TinyUI\\Resource\\combobox\\combobox_down.png");
			SetStyleImage(NORMAL, "D:\\Develop\\TinyUI\\Resource\\combobox\\combobox_normal.png");
			SetStyleImage(HIGHLIGHT, "D:\\Develop\\TinyUI\\Resource\\combobox\\combobox_hover.png");
			SetStyleImage(PUSH, "D:\\Develop\\TinyUI\\Resource\\combobox\\combobox_push.png");
			return TinyVisual::OnCreate();
		}
		HRESULT TinyVisualComboBox::OnDestory()
		{
			return TinyVisual::OnDestory();
		}

		HRESULT	TinyVisualComboBox::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = DOWN;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Redraw(&s);
			m_document->SetCapture(this);
			RECT rect = m_rectangle;
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualComboBox::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = dwFlags & MK_LBUTTON ? DOWN : HIGHLIGHT;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Redraw(&s);
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT TinyVisualComboBox::OnMouseLeave()
		{
			m_dwFlag = NORMAL;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Redraw(&s);
			m_document->ReleaseCapture();
			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualComboBox::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = HIGHLIGHT;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Redraw(&s);
			m_document->SetCapture(NULL);
			EVENT_Click(EventArgs());
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
	}
}
