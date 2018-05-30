#pragma once
#include <Richedit.h>
#include "../Common/TinyDialog.h"

namespace TinyFramework
{
	typedef UINT_PTR(CALLBACK* COMMDLGPROC)(HWND, UINT, WPARAM, LPARAM);
	UINT_PTR CALLBACK CommDlgProc(HWND, UINT, WPARAM, LPARAM);
	/// <summary>
	/// ����Ի���
	/// </summary>
	class TinyFontDialog :public TinyDialog
	{
		DECLARE_DYNAMIC(TinyFontDialog)
	private:
		CHOOSEFONT m_cf;
		LOGFONT m_lf;
		TCHAR m_szStyleName[64];
	public:
		explicit TinyFontDialog(LPLOGFONT lplfInitial = NULL,
			DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
			HDC hDCPrinter = NULL,
			HWND pParentWnd = NULL);
		TinyFontDialog(const CHARFORMAT& charformat,
			DWORD dwFlags = CF_SCREENFONTS,
			HDC hDCPrinter = NULL,
			HWND pParentWnd = NULL);
		virtual INT_PTR DoModal(HWND hParent);
		void GetCurrentFont(LPLOGFONT lplf);
		TinyString GetFaceName() const;
		TinyString GetStyleName() const;
		INT GetSize() const;
		COLORREF GetColor() const;
		INT GetWeight() const;
		BOOL IsStrikeOut() const;
		BOOL IsUnderline() const;
		BOOL IsBold() const;
		BOOL IsItalic() const;
		void GetCharFormat(CHARFORMAT& cf) const;
		DWORD FillInLogFont(const CHARFORMAT& cf);
	};
	/// <summary>
	/// ��ɫ�Ի���
	/// </summary>
	class TinyColorDialog : public TinyDialog
	{
		DECLARE_DYNAMIC(TinyColorDialog)
	public:
		explicit TinyColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0);
		virtual INT_PTR DoModal(HWND hParent);
		COLORREF GetColor() const;
		void SetCustomColors(COLORREF* lpCustColors);
	protected:
		virtual BOOL OnColorOK();
	private:
		CHOOSECOLOR m_cc;
	};
#define _MAX_PATH   260 
#define _MAX_DRIVE  3   
#define _MAX_DIR    256 
#define _MAX_FNAME  256 
#define _MAX_EXT    256

	/// <summary>
	/// �ļ��Ի���
	/// </summary>
	class TinyFileDialog :public TinyDialog
	{
		DECLARE_DYNAMIC(TinyFileDialog)
	private:
		BOOL m_bOpenFileDialog;
		TCHAR m_szFileTitle[_MAX_FNAME];
		TCHAR m_szFileName[_MAX_PATH];
	public:
		OPENFILENAME m_ofn;
		explicit TinyFileDialog(BOOL bOpenFileDialog,
			LPCTSTR lpszDefExt = NULL,
			LPCTSTR lpszFileName = NULL,
			DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			LPCTSTR lpszFilter = NULL,
			DWORD dwSize = 0);
		virtual ~TinyFileDialog();
		virtual INT_PTR DoModal(HWND hParent);
		BOOL IsReadOnly() const;
		void SetFilterText(LPCSTR pzFilter);
		TinyString GetPathName() const;
		TinyString GetFileName() const;
		TinyString GetFileExt() const;
		TinyString GetFileTitle() const;
	private:
		TinyString	m_strFilter;
	};
}