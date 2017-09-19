#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualComboBox.h"
#include "TinyVisualComboBoxHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualComboBox, TinyVisual)

		TinyVisualComboBox::TinyVisualComboBox()
			: m_dwFlag(NORMAL),
			m_dwArrawFlag(NORMAL),
			m_bActive(FALSE),
			m_popupWND(this),
			m_cy(0)
		{

		}
		TinyVisualComboBox::TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree),
			m_dwFlag(NORMAL),
			m_dwArrawFlag(NORMAL),
			m_bActive(FALSE),
			m_popupWND(this),
			m_cy(0)
		{
			ZeroMemory(&m_images, sizeof(m_images));
			ZeroMemory(&m_arraws, sizeof(m_arraws));
			m_onPopupActive.Reset(new Delegate<void(ActiveEventArgs&)>(this, &TinyVisualComboBox::OnPopupActive));
			m_popupWND.EVENT_ACTIVE += m_onPopupActive;
		}
		TinyVisualComboBox::~TinyVisualComboBox()
		{
			m_popupWND.EVENT_ACTIVE -= m_onPopupActive;
		}
		TinyString TinyVisualComboBox::RetrieveTag() const
		{
			return TinyVisualTag::COMBOBOX;
		}
		void TinyVisualComboBox::SetStyleImage(StyleImage type, LPCSTR pzName)
		{
			m_images[(INT)type] = TinyVisualResource::GetInstance()[pzName];
		}
		void TinyVisualComboBox::SetArrowImage(StyleImage type, LPCSTR pzName)
		{
			m_arraws[(INT)type] = TinyVisualResource::GetInstance()[pzName];
		}
		void TinyVisualComboBox::SetSelected(TinyVisualOption* spvis, BOOL bFlag)
		{
			m_popupWND.SetSelected(spvis, bFlag);
		}
		void TinyVisualComboBox::SetSelected(INT index, BOOL bFlag)
		{
			if (index < m_options.GetSize() && index >= 0)
			{
				m_popupWND.SetSelected(m_options[index], bFlag);
			}
		}
		TinyVisualOption* TinyVisualComboBox::GetSelected()
		{
			return	m_popupWND.GetSelected();
		}
		TinyVisualOption* TinyVisualComboBox::AddOption(const TinyString& szValue, const TinyString& szText)
		{
			TinyVisual* spvisParent = m_popupWND.GetDocument()->GetParent(NULL);
			TinyVisualOption* spvis = static_cast<TinyVisualOption*>(m_popupWND.GetDocument()->Create(0, spvisParent->GetChildCount() * DEFAULT_OPTION_HEIGHT, TO_CX(m_rectangle), DEFAULT_OPTION_HEIGHT, TinyVisualTag::OPTION, spvisParent));
			if (spvis != NULL)
			{
				spvis->SetValue(szValue.CSTR());
				spvis->SetText(szText.CSTR());
				spvis->SetTextAlian(DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				spvis->SetHighlightImage("ComboBoxList_highlight");
				spvis->SetTextColor(RGB(0, 0, 0));
				m_options.Add(spvis);
			}
			return spvis;
		}
		TinyArray<TinyVisualOption*>& TinyVisualComboBox::GetOptions()
		{
			return m_options;
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
			return TinyVisual::SetProperty(name, value);
		}
		BOOL TinyVisualComboBox::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			canvas.SetFont(m_hFONT);
			canvas.SetTextColor(m_textColor);
			TinyImage* image = m_images[m_dwFlag];
			if (image != NULL && !image->IsEmpty())
			{
				TinyRectangle srcPaint = image->GetRectangle();
				TinyRectangle srcCenter(srcPaint.left + 5, srcPaint.top + 5, srcPaint.right - 5, srcPaint.bottom - 5);
				canvas.DrawImage(*image, clip, srcPaint, srcCenter);
			}
			TinyImage* arrow = m_arraws[m_dwArrawFlag];
			if (arrow != NULL && !arrow->IsEmpty())
			{
				TinyRectangle srcPaint = arrow->GetRectangle();
				TinyRectangle dstPaint(TinyPoint(clip.right - srcPaint.Width() - 3, clip.top + (clip.Height() - srcPaint.Height()) / 2), srcPaint.Size());
				canvas.DrawImage(*arrow, dstPaint, srcPaint);
			}
			clip.InflateRect(-7, 0, -20, 0);
			canvas.DrawString(GetText(), clip, m_textAlign);
			return TRUE;
		}
		HRESULT	TinyVisualComboBox::OnInitialize()
		{
			m_popupWND.Create(m_document->GetVisualHWND()->Handle(), "D:\\Develop\\TinyUI\\TinyUI\\dropdown.xml");
			m_popupWND.UpdateWindow();
			return TinyVisual::OnInitialize();
		}
		HRESULT TinyVisualComboBox::OnDestory()
		{
			m_popupWND.DestroyWindow();
			return TinyVisual::OnDestory();
		}

		HRESULT	TinyVisualComboBox::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = PUSH;
			TinyRectangle s = m_document->GetWindowRect(this);
			m_document->Redraw(&s);
			m_document->ReleaseCapture();
			TinyPoint screenPos = m_document->GetScreenPos(this);
			screenPos.Offset(2, s.Height() - 1);
			TinyVisual* spvis = m_popupWND.GetDocument()->GetParent(NULL);
			DWORD dwCount = m_options.GetSize();
			if (dwCount > 0)
			{
				TinyRectangle rectangle = spvis->GetRectangle();
				TinySize size = rectangle.Size();
				if (size.cy == 0)
				{
					size.cy = dwCount * DEFAULT_OPTION_HEIGHT;
					spvis->SetSize(size);
				}
				rectangle = spvis->GetRectangle();
				m_popupWND.SetPosition(screenPos, rectangle.Size());
			}
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
				ShowWindow(m_popupWND, SW_HIDE);
				TinyRectangle s = m_document->GetWindowRect(this);
				m_document->Redraw(&s);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		IMPLEMENT_DYNCREATE(TinyVisualOption, TinyVisual);

		TinyVisualOption::TinyVisualOption()
			: m_dwFlag(NORMAL),
			m_bSelected(FALSE)
		{

		}

		TinyVisualOption::TinyVisualOption(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree),
			m_dwFlag(NORMAL),
			m_bSelected(FALSE)
		{
		}
		TinyVisualOption::~TinyVisualOption()
		{
		}
		TinyString TinyVisualOption::RetrieveTag() const
		{
			return TinyVisualTag::OPTION;
		}

		HRESULT TinyVisualOption::OnMouseLeave()
		{
			m_dwFlag = NORMAL;
			m_document->Redraw();
			return TinyVisual::OnMouseLeave();
		}

		HRESULT TinyVisualOption::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			m_dwFlag = HIGHLIGHT;
			m_document->Redraw();
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT TinyVisualOption::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			TinyVisualHWND* pHWND = m_document->GetVisualHWND();
			if (pHWND->IsKindOf(RUNTIME_CLASS(TinyVisualComboBoxHWND)))
			{
				TinyVisualComboBoxHWND*	ps = static_cast<TinyVisualComboBoxHWND*>(pHWND);
				::ShowWindow(ps->Handle(), SW_HIDE);
				ps->SetSelected(this, TRUE);
			}
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
		BOOL TinyVisualOption::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualProperty::VALUE.STR()) == 0)
			{
				SetValue(value.CSTR());
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::OPTIONHIGHLIGHT.STR()) == 0)
			{
				SetHighlightImage(value.CSTR());
				return TRUE;
			}
			return TinyVisual::SetProperty(name, value);
		}
		BOOL TinyVisualOption::IsSelected()
		{
			return m_bSelected;
		}
		void TinyVisualOption::SetSelected(BOOL bFlag)
		{
			m_bSelected = bFlag;
			m_dwFlag = m_bSelected ? HIGHLIGHT : NORMAL;
		}
		void TinyVisualOption::SetValue(LPCSTR pzValue)
		{
			m_szValue = pzValue;
		}
		TinyString TinyVisualOption::GetValue() const
		{
			return m_szValue;
		}
		void TinyVisualOption::SetHighlightImage(LPCSTR pzName)
		{
			m_highlight = TinyVisualResource::GetInstance()[pzName];
		}
		BOOL TinyVisualOption::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			canvas.SetFont(m_hFONT);
			clip.InflateRect({ -1, -1 });
			if (m_dwFlag == NORMAL)
			{
				canvas.SetTextColor(m_textColor);
			}
			else
			{
				canvas.SetTextColor(RGB(255, 255, 255));
				if (m_highlight != NULL && !m_highlight->IsEmpty())
				{
					canvas.DrawImage(*m_highlight, clip, m_highlight->GetRectangle());
				}
			}
			clip.InflateRect({ -8, 0 });
			canvas.DrawString(m_szText, clip, m_textAlign);
			return TRUE;
		}
	}
}
