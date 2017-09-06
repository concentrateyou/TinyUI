#pragma once
#include "TinyVisual.h"
#include "TinyTextHost.h"
#include "TinyVisualScrollbar.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualRichText : public TinyVisual, public TinyVisualFilter
		{
			friend class TinyVisualDocument;
			friend class TinyTextHost;
			DECLARE_DYNAMIC(TinyVisualRichText)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualRichText)
		protected:
			TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualRichText();
			virtual DWORD RetrieveStyle() const;
			TinyString RetrieveTag() const OVERRIDE;
			void SetText(LPCSTR pzText) OVERRIDE;
		public:
			BOOL SetReadonly(BOOL bReadOnly);
			BOOL SetMultiline(BOOL bMultiline);
			BOOL SetPassword(BOOL bPassword, CHAR s);
			BOOL ShowScrollBar(INT bar, BOOL fShow);
			INT GetLineCount();
			INT GetLine(INT nIndex, LPTSTR lpszBuffer);
			INT GetLine(INT nIndex, LPTSTR lpszBuffer, INT nMaxLength);
			INT LineIndex(INT nLine);
			INT LineLength(INT nLine);
			INT GetTextRange(INT nFirst, INT nLast, wstring& wszText);
			LONG GetSelText(LPWSTR lpBuf);
			string GetSelText();
			void GetSel(LONG& nStartChar, LONG& nEndChar);
			void GetSel(CHARRANGE &cr);
			void SetSel(LONG nStartChar, LONG nEndChar);
			void SetSel(CHARRANGE &cr);
			void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE);
			void LimitText(LONG nChars = 0);
			TinyPoint GetCharPos(LONG lChar);
			TinyPoint PosFromChar(UINT nChar);
			LONG LineFromChar(LONG nIndex);
			INT CharFromPos(TinyPoint pt);
			LONG GetLimitText();
			DWORD GetDefaultCharFormat(CHARFORMAT &cf);
			DWORD GetDefaultCharFormat(CHARFORMAT2 &cf);
			DWORD GetSelectionCharFormat(CHARFORMAT &cf);
			DWORD GetSelectionCharFormat(CHARFORMAT2 &cf);
			DWORD GetParaFormat(PARAFORMAT &pf);
			DWORD GetParaFormat(PARAFORMAT2 &pf);
			BOOL SetParaFormat(PARAFORMAT &pf);
			BOOL SetParaFormat(PARAFORMAT2 &pf);
			BOOL SetDefaultCharFormat(CHARFORMAT &cf);
			BOOL SetDefaultCharFormat(CHARFORMAT2 &cf);
			BOOL SetSelectionCharFormat(CHARFORMAT &cf);
			BOOL SetSelectionCharFormat(CHARFORMAT2 &cf);
			BOOL SetWordCharFormat(CHARFORMAT &cf);
			BOOL SetWordCharFormat(CHARFORMAT2 &cf);
		protected:
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			BOOL	OnFilter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) OVERRIDE;
			HRESULT OnCreate() OVERRIDE;
			HRESULT OnInitialize() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			HRESULT OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags) OVERRIDE;
			HRESULT OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags) OVERRIDE;
			HRESULT OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags) OVERRIDE;
			HRESULT OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags) OVERRIDE;
			HRESULT OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage) OVERRIDE;
			HRESULT	OnFocus(BOOL bFlag) OVERRIDE;
			HRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes) OVERRIDE;
		public:
			virtual void OnPosChange(BOOL, INT, INT, INT);
		private:
			TinyTextHost			m_texthost;
			TinyVisualHScrollBar*	m_hscroll;
			TinyVisualVScrollBar*	m_vscroll;
			TinyScopedPtr<Delegate<void(BOOL, INT, INT, INT)>> m_onPosChange;
		};
	}
}



