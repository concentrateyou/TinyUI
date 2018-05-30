#pragma once
#include "../Common/TinyWindowMsg.h"
#include "../Common/TinyString.h"
#include "../Common/TinyApplication.h"
#include "../Common/TinyEvent.h"
#include "../Control/TinyMenu.h"
#include "../Render/TinyGDI.h"

namespace TinyFramework
{
#undef SubclassDialog
	/// <summary>
	/// 对话框基类
	/// </summary>
	class TinyDialog : public TinyHandleHWND
	{
		DECLARE_DYNAMIC(TinyDialog)
	protected:
		TinyLoopThunk	m_thunk;// Thunk类
		INT_PTR			m_iDlgResult;
		BOOL			m_bModal;
		DLGPROC			m_hPrimaryProc;//原始对话框过程
	protected:
		virtual void PreSubclassDialog();
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		virtual void OnFinalMessage(HWND hWND);
		virtual BOOL DestroyWindow();
	private:
		static INT_PTR CALLBACK BeginLoop(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static INT_PTR CALLBACK EndLoop(HWND hWND, UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		TinyDialog();
		virtual ~TinyDialog();
		virtual BOOL Create(HWND hParent, WORD wInteger);
		virtual BOOL Create(HWND hParent, LPCTSTR lpTemplateName);
		virtual INT_PTR DoModal(HWND hParent = ::GetActiveWindow(), WORD wInteger = 0);
		virtual INT_PTR DoModal(HWND hParent = ::GetActiveWindow(), LPCTSTR lpTemplateName = NULL);
		BOOL SubclassDialog(HWND hWND);
		HWND UnsubclassDialog();
		LRESULT DefWindowProc();
		BOOL MapDialogRect(LPRECT lpRect);
		BOOL EndDialog();
		BOOL EndDialog(INT_PTR m_DlgResult);
		BOOL EndDialog(HWND hWND, INT_PTR m_DlgResult);
		BOOL IsModal() const throw();
		DWORD GetDefID() const;
		void SetDefID(UINT nID);
		void NextDlgCtrl() const;
		void PrevDlgCtrl() const;
		void GotoDlgCtrl(HWND hWND);
	};
}


