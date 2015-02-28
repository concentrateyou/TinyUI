#pragma once
#include "TinyControl.h"
#include "../Render/TinyImage.h"

namespace TinyUI
{
#define MINTHUMB_SIZE				  10
#define SM_CXVSCROLL				  12 //��ֱ��������ͷ�Ŀ���
#define SM_CYVSCROLL				  12 //��ֱ��������ͷ�ĸ߶�
	//HITTEST
#define HTSCROLL_NONE                 0x00000000L
#define HTSCROLL_LINEUP               0x00000001L
#define HTSCROLL_LINEDOWN             0x00000002L
#define HTSCROLL_THUMB                0x00000003L
#define HTSCROLL_PAGEUP               0x00000004L
#define HTSCROLL_PAGEDOWN             0x00000005L
	//STATE
#define	SB_STATE_DEFAULT              0x00000000L
#define	SB_STATE_NORMAL               0x00000001L
#define	SB_STATE_HOTTRACKED           0x00000002L
#define	SB_STATE_PRESSED              0x00000003L
#define	SB_STATE_DISABLED             0x00000004L
	typedef struct tagSCROLLCALC {
		RECT	rectangle;
		INT		arrow[2];//LEFT-UP,RIGHT-DOWN
		INT		thumb[2];//LEFT-TOP,RIGHT-BOTTOM
	} SCROLLCALC, *LPSCROLLCALC;
	/// <summary>
	/// �Ի������
	/// </summary>
	class TinyScroll : public TinyControl
	{
	public:
		TinyScroll();
		~TinyScroll();
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	public:
		void SetScrollInfo(SCROLLINFO si);
		void ScrollCalculate(LPSCROLLCALC lpcalc);
		INT  ScrollHitTest(POINT ps);
		void DrawArrow(TinyMemDC dc, INT code, BOOL bMouseDown);
		void DrawThumb(TinyMemDC dc, INT code, BOOL bMouseDown);
		void DrawGroove(TinyMemDC dc);
	private:
		SCROLLINFO	m_si;
		INT			m_iLastingCode;
		INT			m_iTrackingCode;
		BOOL		m_bTracking;
		SIZE		m_size;
	private:
		TinyImage smiley_021;
		/*TinyImage m_arrow_down_hover;
		TinyImage m_arrow_down_normal;
		TinyImage m_arrow_down_press;
		TinyImage m_arrow_up_hover;
		TinyImage m_arrow_up_normal;
		TinyImage m_arrow_up_press;
		TinyImage m_scrollbar_groove;
		TinyImage m_scrollbar_hover;
		TinyImage m_scrollbar_normal;*/
	};
}

