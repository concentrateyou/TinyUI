#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyString.h"
#include "../Common/TinyEvent.h"
#include "../Render/TinyCanvas.h"
#include "TinyVisualCommon.h"
#include "TinyVisualEvent.h"
#include "../XML//tinyxml.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// 可视化元素基类
		/// </summary>
		class TinyVisual : public TinyObject
		{
			friend class TinyVisualDocument;
			friend class TinyVisualBuilder;
			friend class TinyVisualFactory;
			friend class TinyClipCanvas;
			DECLARE_DYNCREATE(TinyVisual)
			DISALLOW_COPY_AND_ASSIGN(TinyVisual)
		protected:
			TinyVisual();
			TinyVisual(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisual();
			virtual TinyString	RetrieveTag() const;
		public:
			TinyVisualDocument*	GetDocument();
			BOOL				IsVisible() const;
			BOOL				IsEnable() const;
			BOOL				IsCapture() const;
			BOOL				IsFocus() const;
			BOOL				IsActive() const;
		public:
			UINT				GetChildCount() const;
			BOOL				Invalidate();
			virtual	HRESULT		SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
			virtual void		SetName(const TinyString& pzName);
			virtual void		SetCursor(const TinyString& szCursor);
			virtual void		SetToolTip(const TinyString& szTitle);
			virtual void		SetText(const TinyString& szText);
			//Box
			virtual void		SetPosition(const TinyPoint& pos);
			virtual void		SetSize(const TinySize& size);
			virtual void		SetMaximumSize(const TinySize& size);
			virtual void		SetMinimumSize(const TinySize& size);
			virtual void		SetClip(HRGN hrgnClip);
			virtual void		SetFont(HFONT hFONT);
			virtual void		SetVisible(BOOL visible);
			virtual void		SetEnable(BOOL enable);
			virtual void		SetPadding(const TinyRectangle& s);
			//背景
			virtual void		SetBackgroundImage(TinyImage* image);
			virtual void		SetBackgroundColor(COLORREF color);
			virtual void		SetBackgroundRectangle(const TinyRectangle& rectangle);
			virtual void		SetBackgroundCenter(const TinyRectangle& center);
			//边框
			virtual void		SetBorderThickness(const TinyRectangle& thickness);
			virtual	void		SetBorderColor(COLORREF color);
			virtual	void		SetBorderStyle(UINT style);
			//文字
			virtual void		SetTextColor(COLORREF color);
			virtual void		SetTextAlian(UINT textAlign);

			TinyString			GetText() const;
			TinyString			GetName() const;
			TinyString			GetToolTip() const;
			TinyPoint			GetPosition() const;
			TinySize			GetSize() const;
			TinySize			GetMaximumSize() const;
			TinySize			GetMinimumSize() const;
			TinyRectangle		GetPadding() const;
			TinyRectangle		GetWindowRect()const;
			TinyRectangle		GetClientRect() const;
			HRGN				GetClip() const;
			HFONT				GetFont() const;
			TinyRectangle		GetBackgroundRectangle() const;
			COLORREF			GetBackgroundColor() const;
			TinyImage*			GetBackgroundImage() const;
			TinyRectangle		GetBackgroundCenter() const;
			COLORREF			GetTextColor() const;
			UINT				GetTextAlian() const;
			TinyRectangle		GetBorderThickness() const;
			COLORREF			GetBorderColor() const;
			UINT				GetBorderStyle() const;

		protected:
			virtual HRESULT		OnCreate();
			virtual HRESULT		OnDestory();
			virtual HRESULT		OnLButtonDown(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnLButtonUp(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnRButtonDown(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnRButtonUp(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnRButtonDBClick(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnMButtonDown(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnMButtonUp(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnMButtonDBClick(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnMouseMove(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags);
			virtual	HRESULT		OnMouseHover(const TinyPoint& pos, DWORD dwFlags);
			virtual HRESULT		OnMouseEnter();
			virtual HRESULT		OnMouseLeave();
			virtual HRESULT		OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);
			virtual HRESULT		OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);
			virtual HRESULT		OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);
			virtual HRESULT		OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage);
			virtual HRESULT		OnFocus(BOOL bFlag);
			virtual HRESULT		OnCapture(BOOL bFlag);
			virtual HRESULT		OnActive(BOOL bFlag);
			virtual BOOL		OnDraw(HDC hDC, const RECT& clip);
		public:
			Event<void(TinyVisual*, EventArgs&)>		EVENT_CREATE;
			Event<void(TinyVisual*, EventArgs&)>		EVENT_DESTORY;
			Event<void(TinyVisual*, EventArgs&)>		EVENT_INITIALIZE;
			Event<void(TinyVisual*, EventArgs&)>		EVENT_CLICK;
			Event<void(TinyVisual*, EventArgs&)>		EVENT_DBCLICK;
			Event<void(TinyVisual*, MouseEventArgs&)>	EVENT_MOUSEMOVE;
			Event<void(TinyVisual*, MouseEventArgs&)>	EVENT_MOUSEWHEEL;
			Event<void(TinyVisual*, MouseEventArgs&)>	EVENT_MOUSEDOWN;
			Event<void(TinyVisual*, MouseEventArgs&)>	EVENT_MOUSEUP;
			Event<void(TinyVisual*, MouseEventArgs&)>	EVENT_MOUSECLICK;
			Event<void(TinyVisual*, MouseEventArgs&)>	EVENT_MOUSEDBCLICK;
			Event<void(TinyVisual*, KeyEventArgs&)>		EVENT_KEYDOWN;
			Event<void(TinyVisual*, KeyEventArgs&)>		EVENT_KEYUP;
			Event<void(TinyVisual*, KeyEventArgs&)>		EVENT_CHAR;
			Event<void(TinyVisual*, FocusEventArgs&)>	EVENT_FOCUS;
			Event<void(TinyVisual*, CaptureEventArgs&)>	EVENT_CAPTURE;
			Event<void(TinyVisual*, ActiveEventArgs&)>	EVENT_ACTIVE;
		protected:
			UINT				m_count;//还是孩子节点个数
			TinyVisual*			m_spvisParent;//父节点
			TinyVisual*			m_spvisNext;//同级下一个兄弟节点
			TinyVisual*			m_spvisChild;//第一个孩子节点
			TinyVisual*			m_spvisOwner;//对于Popup窗口使用
			TinyVisualDocument*	m_document;

			TinyString			m_szName;
			TinyString			m_szText;
			TinyString			m_szToolTip;
			TinyString			m_szCursor;
			TinySize			m_maximumSize;//元素的最大像素大小
			TinySize			m_minimumSize;//元素的最小像素大小
			TinyRectangle		m_rectangle;//相对于父元素区域
			TinyRectangle		m_padding;
			//背景
			TinyRectangle		m_backgroundRectangle;
			TinyRectangle		m_backgroundCenter;
			COLORREF			m_backgroundColor;
			TinyImage*			m_backgroundImage;
			//边框
			TinyRectangle		m_borderThickness;
			COLORREF			m_borderColor;
			UINT				m_borderStyle;

			COLORREF 			m_textColor;
			UINT				m_textAlign;

			HRGN				m_hrgnClip;
			HFONT				m_hFONT;
			BOOL				m_visible;
			BOOL				m_enable;
		};
	}
}



