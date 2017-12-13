#pragma once
#include "../Render/TinyImage.h"
#include "TinyVisualWND.h"
#include "TinyVisualDocument.h"
#include "TinyVisualUtility.h"
#include "TinyVisualShadow.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// 窗口框架类
		/// </summary>
		class TinyVisualFrame : public TinyVisualWND
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualFrame)
		public:
			TinyVisualFrame();
			virtual ~TinyVisualFrame();
			BOOL Create(HWND hParent, LPCSTR pszFile);
			BOOL Create(HWND hParent, LPCTSTR lpTemplateName, LPCSTR pszFile);
			BOOL Create(HWND hParent, WORD wInteger, LPCSTR pszFile);
			INT_PTR DoModal(HWND hParent, WORD wInteger, LPCSTR pszFile);
			INT_PTR DoModal(HWND hParent, LPCTSTR lpTemplateName, LPCSTR pszFile);
			LPCSTR	RetrieveClassName() OVERRIDE;
			LPCSTR	RetrieveTitle() OVERRIDE;
			HICON	RetrieveIcon() OVERRIDE;
			DWORD	RetrieveStyle() OVERRIDE;
			DWORD	RetrieveExStyle() OVERRIDE;
		public:
			BOOL AddFilter(TinyVisualFilter* ps);
			BOOL RemoveFilter(TinyVisualFilter* ps);
			void AllowTracking(BOOL bAllow);
			TinyVisualShadow&	GetShadow();
			TinyVisualDocument*	GetDocument();
			virtual void OnInitialize() = 0;
			virtual void OnUninitialize() = 0;
		public:
			//消息
			BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) OVERRIDE;
			LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnMButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnMButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnMButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnExitSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			//NC
			LRESULT OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			//Other
			LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		private:
			BOOL	Initialize();
			void	Uninitialize();
		protected:
			BOOL								m_bAllowTracking;
			BOOL								m_bMouseTracking;
			TinyPoint							m_pos;
			TinySize							m_size;
			TinyString							m_szSkinFile;//资源文件
			TinyVisualFilters					m_mFilters;
			TinyVisualBuilder					m_builder;
			TinyScopedPtr<TinyVisualDC>			m_visualDC;
			TinyScopedPtr<TinyVisualDocument>	m_document;
			TinyVisualShadow					m_shadow;
		};
	}
}



