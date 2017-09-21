#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualConst
		{
		public:
			static const TinyString LEFT;
			static const TinyString TOP;
			static const TinyString RIGHT;
			static const TinyString BOTTOM;
			static const TinyString CENTER;
			static const TinyString STRETCH;
		};
		/// <summary>
		/// 元素属性常量
		/// </summary>
		class TinyVisualProperty
		{
		public:
			static const TinyString NAME;
			static const TinyString TEXT;
			static const TinyString TOOLTIP;
			static const TinyString MAXSIZE;
			static const TinyString MINSIZE;
			static const TinyString POSITION;
			static const TinyString SIZE;
			static const TinyString VISIBLE;
			static const TinyString ENABLE;
			static const TinyString BACKGROUNDIMAGE;
			static const TinyString BACKGROUNDCOLOR;
			static const TinyString BACKGROUNDCENTER;
			static const TinyString BACKGROUNDSIZE;
			static const TinyString BACKGROUNDPOSITION;
			static const TinyString BORDERWIDTH;
			static const TinyString BORDERCOLOR;
			static const TinyString BORDERSTYLE;

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

			static const TinyString ALIGNMENT;
			static const TinyString VALUE;
			static const TinyString OPTIONHIGHLIGHT;
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
			static const TinyString ANIMATE;
			static const TinyString TAB;
			static const TinyString BUTTON;
			static const TinyString PANEL;
			static const TinyString LIST;
			static const TinyString LISTITEM;
			static const TinyString STACKPANEL;
			static const TinyString HBOXLAYOUT;
			static const TinyString VBOXLAYOUT;
			static const TinyString LISTBOX;
			static const TinyString TOOLBAR;
			static const TinyString TREEVIEW;
			static const TinyString HSCROLLBAR;
			static const TinyString VSCROLLBAR;
			static const TinyString COMBOBOX;
			static const TinyString OPTION;
			static const TinyString TEXTBOX;
			static const TinyString STRETCH;
			static const TinyString NATIVEWINDOW;
			static const TinyString CONTEXT;
			static const TinyString IMAGE;
			static const TinyString ADD;
		};
		/// <summary>
		/// 缓存DC
		/// </summary>
		class TinyVisualDC : public TinyHandleHDC
		{
		public:
			TinyVisualDC(HWND hWND);
			~TinyVisualDC();
			void SetSize(INT cx, INT cy);
			HDC	 GetMemDC() const;
			BOOL Render(const RECT& s);
			BOOL Render(const RECT& s, INT x, INT y);
			BOOL RenderLayer(const RECT& s);
			BOOL RenderLayer(const RECT& s, INT x, INT y);
		protected:
			HWND		m_hWND;
			TinySize	m_size;
			HDC			m_hMemDC;
			HBITMAP		m_hMemBitmap;
			HBITMAP		m_OldBitmap;
		};
	}
}



