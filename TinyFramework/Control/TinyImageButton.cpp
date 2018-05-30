#include "TinyImageButton.h"


namespace TinyFramework
{
	IMPLEMENT_DYNAMIC(TinyImageButton, TinyButton);

	TinyImageButton::TinyImageButton()
		:m_state(0)
	{
	}

	TinyImageButton::~TinyImageButton()
	{
	}
	BOOL TinyImageButton::SetFiles(const TinyString& szFile1, const TinyString& szFile2, const TinyString& szFile3)
	{
		if (!PathFileExists(szFile1.CSTR()))
			return FALSE;
		if (!PathFileExists(szFile2.CSTR()))
			return FALSE;
		if (!PathFileExists(szFile3.CSTR()))
			return FALSE;
		if (!m_images[0].Open(szFile1.CSTR()))
			return FALSE;
		if (!m_images[1].Open(szFile2.CSTR()))
			return FALSE;
		if (!m_images[2].Open(szFile3.CSTR()))
			return FALSE;
		return TRUE;
	}

	INT TinyImageButton::GetTextAlignment() const
	{

		DWORD bHorz;
		DWORD bVert;
		DWORD dwStyle = GetStyle();
		bHorz = dwStyle & (BS_LEFT | BS_RIGHT | BS_CENTER);
		bVert = dwStyle & (BS_TOP | BS_BOTTOM | BS_VCENTER);
		if (!bHorz || !bVert)
		{
			if (IsPushButton())
			{
				if (!bHorz)
					bHorz = BS_CENTER;
			}
			else
			{
				if (!bHorz)
					bHorz = BS_LEFT;
			}

			if (!bVert)
				bVert = BS_VCENTER;
		}
		return bHorz | bVert;
	}

	BOOL TinyImageButton::IsPushButton() const
	{
		DWORD dwStyle = GetStyle();
		switch (GetButtonStyle())
		{
		case LOBYTE(BS_PUSHBUTTON):
		case LOBYTE(BS_DEFPUSHBUTTON):
		case LOBYTE(BS_OWNERDRAW):
			return TRUE;
		default:
			if ((dwStyle & BS_PUSHLIKE) == BS_PUSHLIKE)
				return TRUE;
			break;
		}
		return FALSE;
	}

	void TinyImageButton::SetTextAlignment(DWORD dwAlignment)
	{
		ModifyStyle(BS_LEFT | BS_CENTER | BS_RIGHT | BS_TOP | BS_BOTTOM | BS_VCENTER, dwAlignment, 0);
	}

	LRESULT TinyImageButton::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return FALSE;
	}

	LRESULT TinyImageButton::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT ps = { 0 };
		HDC hDC = BeginPaint(m_hWND, &ps);
		this->OnDraw(hDC);
		EndPaint(m_hWND, &ps);
		return FALSE;
	}

	LRESULT TinyImageButton::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_state = 1;
		this->Invalidate();
		return TinyButton::OnMouseMove(uMsg, wParam, lParam, bHandled);
	}

	LRESULT TinyImageButton::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_state = 0;
		this->Invalidate();
		return TinyButton::OnMouseLeave(uMsg, wParam, lParam, bHandled);
	}

	LRESULT TinyImageButton::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_state = 2;
		this->Invalidate();
		return TinyButton::OnLButtonDown(uMsg, wParam, lParam, bHandled);
	}

	LRESULT TinyImageButton::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_state = 0;
		this->Invalidate();
		return TinyButton::OnLButtonUp(uMsg, wParam, lParam, bHandled);
	}
	void TinyImageButton::CalcRect(HDC hDC, const TinyString& szText, TinyRectangle& rectangle, UINT& wFlag)
	{
		wFlag = DT_SINGLELINE | DT_LEFT | DT_TOP;
		if (this->GetExStyle() & WS_EX_RTLREADING)
			wFlag |= DT_RTLREADING;
		if (this->GetStyle() & BS_MULTILINE)
		{
			wFlag |= DT_WORDBREAK | DT_EDITCONTROL;
			wFlag &= ~DT_SINGLELINE;
		}
	}
	void TinyImageButton::OnDraw(HDC hDC)
	{
		TinyMemDC dc(hDC, m_images[m_state]);
		dc.SetBkMode(TRANSPARENT);
		TinyString szText = this->GetText();
		if (!szText.IsEmpty())
		{
			HFONT hOldF = (HFONT)dc.SelectObject(GetFont());
			TinyRectangle rectangle;
			GetClientRect(&rectangle);
			UINT sFlag = DT_SINGLELINE | DT_LEFT | DT_TOP;
			if (this->GetExStyle() & WS_EX_RTLREADING)
				sFlag |= DT_RTLREADING;
			if (this->GetStyle() & BS_MULTILINE)
			{
				sFlag |= DT_WORDBREAK | DT_EDITCONTROL;
				sFlag &= ~DT_SINGLELINE;
			}
			dc.DrawText(szText.CSTR(), szText.GetSize(), &rectangle, sFlag);
			dc.SelectObject(hOldF);
			TinyRectangle imageRect = m_images[m_state].GetRectangle();
			dc.Render(rectangle, imageRect, TRUE);
		}
	}
}

