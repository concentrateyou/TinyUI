#pragma once
#include "../Render/TinyImage.h"
#include "TinyVisualWND.h"
#include "TinyVisualDocument.h"
#include "TinyVisualShadow.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// 缓存DC
		/// </summary>
		class TinyVisualDC : public TinyHandleHDC
		{
			DECLARE_DYNAMIC(TinyVisualDC)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualDC)
		public:
			TinyVisualDC(HWND hWND);
			virtual ~TinyVisualDC();
			BOOL		IsEmpty() const;
			HDC			GetCompatibleDC() const;
			HBITMAP		GetCompatibleBitmap() const;
			void		SetSize(INT cx, INT cy);
			TinySize	GetSize() const;
			BOOL		Render(INT dstX, INT dstY, INT dstCX, INT dstCY, INT srcX, INT srcY);
			BOOL		Render(INT dstX, INT dstY, INT dstCX, INT dstCY, INT srcX, INT srcY, INT srcCX, INT srcCY);
			BOOL		Update();
		public:
			static TinySize	GetTextExtent(HDC hDC, LPCSTR lpszString, INT count);
		protected:
			HWND		m_hWND;
			HDC			m_hMemDC;
			HBITMAP		m_hBitmap;
			TinySize	m_size;
		};
		/// <summary>
		/// 裁剪的DC在当前元素最小的矩形上绘制
		/// </summary>
		class TinyClipCanvas : public TinyCanvas
		{
		public:
			TinyClipCanvas(HDC hDC, TinyVisual* spvis, const RECT& rcPaint);
			virtual ~TinyClipCanvas();
		private:
			HRGN	m_hRGN;
			HRGN	m_hOldRGN;
		};
		/// <summary>
		/// 消息过滤器
		/// </summary>
		class NO_VTABLE TinyVisualFilter
		{
		public:
			virtual BOOL OnFilter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) = 0;
		};
		/// <summary>
		/// 可视化消息筛选器集合
		/// </summary>
		class TinyVisualFilters
		{
		public:
			TinyVisualFilters();
			~TinyVisualFilters();
			BOOL Add(TinyVisualFilter* element);
			BOOL Remove(TinyVisualFilter* element);
			BOOL RemoveAt(INT index);
			void RemoveAll();
			INT  GetSize() const;
			const TinyVisualFilter* operator[] (INT index) const;
			TinyVisualFilter* operator[] (INT index);
			INT Lookup(TinyVisualFilter* element) const;
		private:
			TinyArray<TinyVisualFilter*> m_array;
		};
		/// <summary>
		/// 窗口框架类
		/// </summary>
		class TinyVisualWindowless : public TinyVisualWND
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualWindowless)
		public:
			TinyVisualWindowless();
			virtual ~TinyVisualWindowless();
			BOOL	Create(HWND hParent, LPCSTR pszFile);
			BOOL	Create(HWND hParent, LPCTSTR lpTemplateName, LPCSTR pszFile);
			BOOL	Create(HWND hParent, WORD wInteger, LPCSTR pszFile);
			INT_PTR DoModal(HWND hParent, WORD wInteger, LPCSTR pszFile);
			INT_PTR DoModal(HWND hParent, LPCTSTR lpTemplateName, LPCSTR pszFile);
			LPCSTR	RetrieveClassName() OVERRIDE;
			LPCSTR	RetrieveTitle() OVERRIDE;
			HICON	RetrieveIcon() OVERRIDE;
			DWORD	RetrieveStyle() OVERRIDE;
			DWORD	RetrieveExStyle() OVERRIDE;
		public:
			BOOL				AddFilter(TinyVisualFilter* ps);
			BOOL				RemoveFilter(TinyVisualFilter* ps);
			void				AllowTracking(BOOL bAllow);
			TinyVisualShadow&	GetShadow();
			TinyVisualDocument&	GetDocument();
			virtual void		OnInitialize() = 0;
			virtual void		OnUninitialize() = 0;
		public:
			//消息
			BOOL	ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) OVERRIDE;
			LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
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
			void	Draw(RECT *lpRect);
		protected:
			BOOL								m_bAllowTracking;
			BOOL								m_bMouseTracking;
			TinyPoint							m_position;
			TinySize							m_size;
			TinyVisualFilters					m_mFilters;
			TinyVisualDocument					m_document;
			TinyScopedPtr<TinyVisualDC>			m_visualDC;
			TinyVisualShadow					m_shadow;
		};
	}
}



