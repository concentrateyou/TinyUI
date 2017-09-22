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
			COUNT = PUSH + 1
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
		/// ���ӻ�Ԫ�ػ���
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
			TinyImage*			GetBackgroundImage();
			TinyRectangle		GetBackgroundCenter() const;
			TinySize			GetBackgroundSize() const;
			TinyPoint			GetBackgroundPosition() const;
			TinyColor			GetBackgroundColor() const;
			TinyColor			GetBorderColor() const;
			INT					GetBorderThickness() const;
			INT					GetBorderStyle() const;
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
			DWORD				GetChildCount() const;
			virtual	HRESULT		SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
			virtual void		OnSizeChange(const TinySize&, const TinySize&);
			virtual void		OnPosChange(const TinyPoint&, const TinyPoint&);
		public:
			virtual ~TinyVisual();
			virtual TinyString	RetrieveTag() const;
			virtual void		SetName(const TinyString& pzName);
			virtual void		SetToolTip(const TinyString& pzTitle);
			virtual void		SetText(const TinyString& pzText);
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
			virtual void		SetBackgroundImage(const TinyString& szName);
			virtual void		SetBackgroundImage(TinyImage* image);
			virtual void		SetBackgroundColor(COLORREF color);
			virtual void		SetBackgroundCenter(const TinyRectangle& center);
			virtual void		SetBackgroundSize(const TinySize& size);
			virtual void		SetBackgroundPosition(const TinyPoint& pos);
			virtual void		SetBorderColor(COLORREF color);
			virtual void		SetBorderThickness(INT cx);
			virtual void		SetBorderStyle(INT style);
		protected:
			virtual BOOL		OnDraw(HDC hDC, const RECT& clip);
			virtual HRESULT		OnCreate();
			virtual HRESULT		OnDestory();
			virtual HRESULT		OnInitialize();
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
			virtual BOOL		SetProperty(const TinyString& name, const TinyString& value);
			TinyString			GetProperty(const TinyString& name);
			BOOL				IsProperty(const TinyString& name);
		public:
			Event<void(EventArgs&)>			EVENT_CREATE;
			Event<void(EventArgs&)>			EVENT_DESTORY;
			Event<void(EventArgs&)>			EVENT_INITIALIZE;
			Event<void(EventArgs&)>			EVENT_CLICK;
			Event<void(EventArgs&)>			EVENT_DBCLICK;
			Event<void(MouseEventArgs&)>	EVENT_MOUSEMOVE;
			Event<void(MouseEventArgs&)>	EVENT_MOUSEWHEEL;
			Event<void(MouseEventArgs&)>	EVENT_MOUSEDOWN;
			Event<void(MouseEventArgs&)>	EVENT_MOUSEUP;
			Event<void(MouseEventArgs&)>	EVENT_MOUSECLICK;
			Event<void(MouseEventArgs&)>	EVENT_MOUSEDBCLICK;
			Event<void(KeyEventArgs&)>		EVENT_KEYDOWN;
			Event<void(KeyEventArgs&)>		EVENT_KEYUP;
			Event<void(KeyEventArgs&)>		EVENT_CHAR;
			Event<void(FocusEventArgs&)>	EVENT_FOCUS;
			Event<void(CaptureEventArgs&)>	EVENT_CAPTURE;
			Event<void(ActiveEventArgs&)>	EVENT_ACTIVE;
		protected:
			DWORD				m_dwCount;//���Ǻ��ӽڵ����
			TinyVisualDocument*	m_document;
			TinyVisual*			m_spvisNext;//ͬ����һ���ֵܽڵ�
			TinyVisual*			m_spvisParent;//���ڵ�
			TinyVisual*			m_spvisChild;//��һ�����ӽڵ�
			TinyVisual*			m_spvisOwner;//����Popup����ʹ��
			TinyString			m_szName;
			TinyString			m_szText;
			TinyString			m_szToolTip;
			TinySize			m_maximumSize;//Ԫ�ص�������ش�С
			TinySize			m_minimumSize;//Ԫ�ص���С���ش�С
			TinyRectangle		m_rectangle;//����ڸ�Ԫ������
			TinyRectangle		m_backgroundCenter;//��ʾ����ͼƬ����
			TinyImage*			m_backgroundImage;//����ͼƬ
			TinyColor			m_backgroundColor;//������ɫ
			TinySize			m_backgroundSize;//������С
			TinyPoint			m_backgroundPosition;
			INT					m_borderThickness;//�߿���
			TinyColor			m_borderColor;//�߿���ɫ
			INT					m_borderStyle;//�߿���ʽ
			TinyColor 			m_textColor;
			UINT				m_textAlign;
			BOOL				m_visible;
			BOOL				m_enable;
			HRGN				m_hrgnClip;
			HFONT				m_hFONT;
			TinyMap<TinyString, TinyString>	m_propertys;//����Map
		};
	}
}



