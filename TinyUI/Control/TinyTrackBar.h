#pragma once
#include "TinyControl.h"

namespace TinyUI
{
	class TinyTrackBar :public TinyControl
	{
	public:
		TinyTrackBar();
		virtual ~TinyTrackBar();
		virtual LPCSTR RetrieveClassName();
		virtual LPCSTR RetrieveTitle();
		virtual DWORD RetrieveStyle();
		virtual DWORD RetrieveExStyle();
		virtual BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
	public:
		INT GetLineSize() const;
		INT SetLineSize(INT nSize);
		INT GetPageSize() const;
		INT SetPageSize(INT nSize);
		INT GetRangeMax() const;
		INT GetRangeMin() const;
		void GetRange(INT& nMin, INT& nMax) const;
		void SetRangeMin(INT nMin, BOOL bRedraw = FALSE);
		void SetRangeMax(INT nMax, BOOL bRedraw = FALSE);
		void SetRange(INT nMin, INT nMax, BOOL bRedraw = FALSE);
		void GetSelection(INT& nMin, INT& nMax) const;
		void SetSelection(INT nMin, INT nMax);
		void GetChannelRect(LPRECT lprc) const;
		void GetThumbRect(LPRECT lprc) const;
		INT GetPos() const;
		void SetPos(INT nPos);
		UINT GetNumTics() const;
		DWORD* GetTicArray() const;
		INT GetTic(INT nTic) const;
		INT GetTicPos(INT nTic) const;
		BOOL SetTic(INT nTic);
		void SetTicFreq(INT nFreq);
		HWND GetBuddy(BOOL fLocation = TRUE) const;
		HWND SetBuddy(HWND pWndBuddy, BOOL fLocation = TRUE);
		HWND GetToolTips() const;
		void SetToolTips(HWND pWndTip);
		INT SetTipSide(INT nLocation);
		void SetThumbLength(INT nLength);
		INT GetThumbLength() const;
		void ClearSel(BOOL bRedraw = FALSE);
		void ClearTics(BOOL bRedraw = FALSE);
	public:
		LRESULT OnNotifyReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		Event<void(void*, INT)> EVENT_POSCHANGING;
	private:
		INT m_currentPos;
	};
}



