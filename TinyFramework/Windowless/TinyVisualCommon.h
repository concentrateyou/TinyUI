#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "../Common/TinyTuple.h"

namespace TinyFramework
{
	namespace Windowless
	{
#define LY_PER_INCH   1440
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
		LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch);
		LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch);
		LONG DXtoHimetricX(LONG dx, LONG xPerInch);
		LONG DYtoHimetricY(LONG dy, LONG yPerInch);

#define DEFAULT_COLOR	0x00FFFFFF

#define TINY_SM_CXSCROLL				12 //滚动条箭头的宽度
#define TINY_SM_CYSCROLL				12 //滚动条箭头的高度
#define TINY_SCROLL_MINTHUMB_SIZE		10
#define TINY_SCROLL_MARGIN_LEFT			0 
#define TINY_SCROLL_MARGIN_TOP			0 
#define TINY_SCROLL_MARGIN_RIGHT		0 
#define TINY_SCROLL_MARGIN_BOTTOM		0 
#define SB_TIMER_DELAY					65533
#define SB_TIMER_SCROLL					65534
#define SB_SCROLL_DELAY					300
#define SB_SCROLL_INTERVAL				40

#define HTSCROLL_NONE                 0x00000000L
#define HTSCROLL_LINEUP               0x00000001L
#define HTSCROLL_LINEDOWN             0x00000002L
#define HTSCROLL_THUMB                0x00000003L
#define HTSCROLL_PAGEUP               0x00000004L
#define HTSCROLL_PAGEDOWN             0x00000005L

#define ITEM_NORMAL		0x0000
#define ITEM_HIGHLIGHT	0x0001
#define ITEM_CHECKED	0x0002
#define ITEM_SEPARATOR	0x0004
#define ITEM_CHILD		0x0008


#define TestF(flag)		((m_dwFlag & flag) != 0)
#define SetF(flag)		(m_dwFlag |= flag)
#define ClrF(flag)		(m_dwFlag &= ~flag)
		/// <summary>
		/// 元素属性常量
		/// </summary>
		class TinyVisualPropertyConst
		{
		public:
			static const TinyString NAME;
			static const TinyString TEXT;
			static const TinyString TOOLTIP;
			static const TinyString MAXSIZE;
			static const TinyString MINSIZE;
			static const TinyString POSITION;
			static const TinyString SIZE;

			static const TinyString ORIENTATION;

			static const TinyString PADDING;
			static const TinyString MARGIN;

			static const TinyString VISIBLE;
			static const TinyString ENABLE;
			static const TinyString CURSOR;
			static const TinyString BACKGROUNDIMAGE;
			static const TinyString BACKGROUNDCOLOR;
			static const TinyString BACKGROUNDCENTER;
			static const TinyString BACKGROUNDSIZE;
			static const TinyString BACKGROUNDPOSITION;
			static const TinyString BORDERWIDTH;
			static const TinyString BORDERCOLOR;
			static const TinyString BORDERSTYLE;
			static const TinyString BORDERIMAGE;
			static const TinyString BORDERCENTER;

			static const TinyString	FONTFAMILY;
			static const TinyString	FONTSIZE;
			static const TinyString	FONTWEIGHT;
			static const TinyString FONTSTYLE;

			static const TinyString TEXTCOLOR;
			static const TinyString TEXTALIGN;
			static const TinyString TEXTDECORATION;
			static const TinyString MULTILINE;
			static const TinyString READONLY;

			static const TinyString IMAGENORMAL;
			static const TinyString IMAGEHIGHLIGHT;
			static const TinyString IMAGEDOWN;
			static const TinyString IMAGEPUSH;

			static const TinyString	IMAGE;
			static const TinyString	IMAGELIST;

			static const TinyString ARROWNORMAL;
			static const TinyString ARROWDOWN;
			static const TinyString ARROWHIGHLIGHT;
			static const TinyString ARROWPUSH;

			static const TinyString ARROW1NORMAL;
			static const TinyString ARROW1HIGHLIGHT;
			static const TinyString ARROW1DOWN;

			static const TinyString ARROW2NORMAL;
			static const TinyString ARROW2HIGHLIGHT;
			static const TinyString ARROW2DOWN;

			static const TinyString SCROLLBARGROOVE;
			static const TinyString SCROLLBARNORMAL;
			static const TinyString SCROLLBARHIGHLIGHT;

			static const TinyString HORIZONTALALIGNMENT;
			static const TinyString VERTICALALIGNMENT;
			static const TinyString VALUE;
			static const TinyString OPTIONHIGHLIGHT;

			static const TinyString SHADOWIMAGE;
			static const TinyString SHADOWBOX;
		};
		/// <summary>
		/// Tag常量
		/// </summary>
		class TinyVisualTag
		{
		public:
			static const TinyString WINDOW;
			static const TinyString SYSCAPTION;
			static const TinyString LABEL;
			static const TinyString ANIMATION;
			static const TinyString TAB;
			static const TinyString BUTTON;
			static const TinyString PANEL;
			static const TinyString STACKPANEL;
			static const TinyString LIST;
			static const TinyString LISTITEM;
			static const TinyString HBOXLAYOUT;
			static const TinyString VBOXLAYOUT;
			static const TinyString LISTBOX;
			static const TinyString TOOLBAR;
			static const TinyString TREEVIEW;
			static const TinyString HSCROLLBAR;
			static const TinyString VSCROLLBAR;
			static const TinyString COMBOBOX;
			static const TinyString COMBOBOXITEM;
			static const TinyString OPTION;
			static const TinyString TEXTBOX;
			static const TinyString STRETCH;
			static const TinyString NATIVE;
			static const TinyString MENUITEM;
			static const TinyString CONTEXT;
			static const TinyString IMAGE;
			static const TinyString ADD;
		};
		/// <summary>
		/// 光标
		/// </summary>
		class TinyVisualCursor
		{
		public:
			static const TinyString AUTO;
			static const TinyString HAND;
			static const TinyString TEXT;
			static const TinyString MOVE;
			static const TinyString POINTER;
			static const TinyString WAIT;
			static const TinyString HELP;
			static const TinyString ERESIZE;
			static const TinyString NERESIZE;
			static const TinyString NWRESIZE;
			static const TinyString NRESIZE;
			static const TinyString SERESIZE;
			static const TinyString SWRESIZE;
			static const TinyString SRESIZE;
			static const TinyString WRESIZE;
		};
		LPSTR CursorFromString(TinyString& str);
	}
}



