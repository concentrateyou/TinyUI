#pragma once
#include "../Common/TinyString.h"
#include "../Common/TinyEvent.h"
#include "../Render/TinyCanvas.h"
#include "../XML//tinyxml.h"
#include "TinyVisualEvent.h"
#include "TinyVisualUtility.h"

namespace TinyUI
{
	namespace Windowless
	{
		enum StyleImage
		{
			NORMAL = 0,
			HIGHLIGHT = 1,
			DOWN = 2,
			PUSH = 3,
		};
		enum Alignment
		{
			CENTER,
			LEFT,
			RIGHT,
			TOP,
			BOTTOM,
			STRETCH
		};
		/// <summary>
		/// 可视化元素基类
		/// </summary>
		class TinyVisual : public TinyObject
		{
			friend class TinyVisualDocument;
			friend class TinyVisualBuilder;
			friend class TinyVisualFactory;
			friend class TinyClipCanvas;
			DECLARE_DYNAMIC(TinyVisual)
			DISALLOW_COPY_AND_ASSIGN(TinyVisual)
		protected:
			TinyVisual(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			HWND				Handle() const;
			TinyString			GetText() const;
			TinyString			GetName() const;
			TinyString			GetToolTip() const;
			TinyPoint			GetPosition() const;
			TinySize			GetSize() const;
			TinySize			GetMaximumSize() const;
			TinySize			GetMinimumSize() const;
			TinyRectangle		GetRectangle() const;
			TinyRectangle		GetWindowRect()const;
			TinyRectangle		GetClientRect() const;
			HRGN				GetClip() const;
			HFONT				GetFont() const;
			BOOL				IsVisible() const;
			BOOL				IsEnable() const;
			BOOL				IsCapture() const;
			BOOL				IsFocus() const;
			BOOL				IsActive() const;
			BOOL				Invalidate();
		public:
			TinyVisualDocument*	GetDocument();
			virtual	HRESULT		SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
			virtual void		OnSizeChange(const TinySize&, const TinySize&);
			virtual void		OnPosChange(const TinyPoint&, const TinyPoint&);
		public:
			virtual ~TinyVisual();
			virtual TinyString	RetrieveTag() const = 0;
			virtual BOOL		IsLayout() const;
			virtual void		SetName(LPCSTR pzName);
			virtual void		SetToolTip(LPCSTR pzTitle);
			virtual void		SetText(LPCSTR pzText);
			virtual void		SetPosition(const TinyPoint& pos);
			virtual void		SetSize(const TinySize& size);
			virtual void		SetMaximumSize(const TinySize& size);
			virtual void		SetMinimumSize(const TinySize& size);
			virtual void		SetClip(HRGN hrgnClip);
			virtual void		SetFont(HFONT hFONT);
			virtual void		SetVisible(BOOL visible);
			virtual void		SetEnable(BOOL enable);
			virtual void		SetTextColor(COLORREF color);
			virtual void		SetTextAlian(UINT align);
		protected:
			virtual BOOL		OnDraw(HDC hDC, const RECT& clip);
			virtual HRESULT		OnCreate();
			virtual HRESULT		OnDestory();
			virtual HRESULT		OnMouseMove(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags);
			virtual HRESULT		OnMouseEnter();
			virtual HRESULT		OnMouseLeave();
			virtual HRESULT		OnLButtonDown(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnLButtonUp(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnRButtonDown(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnRButtonUp(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnRButtonDBClick(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnMButtonDown(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnMButtonUp(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnMButtonDBClick(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);
			virtual HRESULT		OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);
			virtual HRESULT		OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);
			virtual HRESULT		OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage);
			virtual HRESULT		OnFocus(BOOL bFlag);
			virtual HRESULT		OnCapture(BOOL bFlag);
			virtual HRESULT		OnActive(BOOL bFlag);
		public:
			virtual HRESULT		SetProperty(const TinyString& name, const TinyString& value);
		public:
			Event<void(EventArgs&)>			EVENT_Click;
			Event<void(MouseEventArgs&)>	EVENT_MouseMove;
			Event<void(MouseEventArgs&)>	EVENT_MouseWheel;
			Event<void(MouseEventArgs&)>	EVENT_MouseDown;
			Event<void(MouseEventArgs&)>	EVENT_MouseUp;
			Event<void(MouseEventArgs&)>	EVENT_MouseClick;
			Event<void(MouseEventArgs&)>	EVENT_MouseDoubleClick;
			Event<void(KeyEventArgs&)>		EVENT_KeyDown;
			Event<void(KeyEventArgs&)>		EVENT_KeyUp;
			Event<void(KeyEventArgs&)>		EVENT_Char;
			Event<void(FocusEventArgs&)>	EVENT_Focus;
			Event<void(CaptureEventArgs&)>	EVENT_Capture;
			Event<void(ActiveEventArgs&)>	EVENT_Active;
		protected:
			TinyVisualDocument*	m_document;
			TinyVisual*			m_spvisNext;//同级下一个兄弟节点
			TinyVisual*			m_spvisParent;//父节点
			TinyVisual*			m_spvisChild;//第一个孩子节点
			TinyVisual*			m_spvisOwner;//对于Popup窗口使用
			TinyString			m_strName;
			TinyString			m_strText;
			TinyString			m_strToolTip;
			TinySize			m_maximumSize;//元素的最大像素大小
			TinySize			m_minimumSize;//元素的最小像素大小
			TinyRectangle		m_rectangle;//相对于父元素区域
			COLORREF 			m_textColor;
			UINT				m_textAlign;
			BOOL				m_visible;
			BOOL				m_enable;
			DWORD				m_dwCount;//还是孩子节点个数
			HRGN				m_hrgnClip;
			HFONT				m_hFONT;
		};
	}
}



