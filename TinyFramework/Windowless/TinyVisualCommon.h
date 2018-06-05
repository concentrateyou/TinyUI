#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// Ԫ�����Գ���
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

			static const TinyString ALIGNMENT;
			static const TinyString VALUE;
			static const TinyString OPTIONHIGHLIGHT;

			static const TinyString SHADOWIMAGE;
			static const TinyString SHADOWBOX;
		};
		/// <summary>
		/// Tag����
		/// </summary>
		class TinyVisualTagConst
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
			static const TinyString OPTION;
			static const TinyString TEXTBOX;
			static const TinyString STRETCH;
			static const TinyString NATIVEWINDOW;
			static const TinyString MENUITEM;
			static const TinyString CONTEXTMENU;
			static const TinyString CONTEXT;
			static const TinyString IMAGE;
			static const TinyString ADD;
		};
		/// <summary>
		/// ���
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
		/// <summary>
		/// ����DC
		/// </summary>
		class TinyVisualDC : public TinyHandleHDC
		{
			DECLARE_DYNAMIC(TinyVisualDC)
		public:
			TinyVisualDC(HWND hWND);
			virtual ~TinyVisualDC();
			void SetSize(INT cx, INT cy);
			HDC	 GetMemDC() const;
			BOOL Render(const RECT& s);
			BOOL Render(const RECT& s, INT x, INT y);
			BOOL RenderLayer(const RECT& s);
			BOOL RenderLayer(const RECT& s, INT x, INT y);
		protected:
			HWND		m_hWND;
			HDC			m_hMemDC;
			HBITMAP		m_hBitmap;
			HBITMAP		m_hOldBitmap;
			TinySize	m_size;
		};
		/// <summary>
		/// ����
		/// </summary>
		class TinyVisualProperty
		{
		public:
			TinyVisualProperty(const TinyString& name, const TinyString& value);
			TinyVisualProperty(const TinyVisualProperty&);
			~TinyVisualProperty();
			void operator=(const TinyVisualProperty&);
			const TinyString& name() const;
			const TinyString& value() const;
		private:
			TinyString	m_name;
			TinyString	m_value;
		};
		enum VARIANT_TYPE
		{
			VARIANT_TYPE_EMPTY,
			VARIANT_TYPE_RECT,
			VARIANT_TYPE_POINT,
			VARIANT_TYPE_SIZE,
			VARIANT_TYPE_OBJECT,
			VARIANT_TYPE_FONT,
			VARIANT_TYPE_STRING,
			VARIANT_TYPE_BYTE,
			VARIANT_TYPE_CHAR,
			VARIANT_TYPE_WORD,
			VARIANT_TYPE_DWORD,
			VARIANT_TYPE_SHORT,
			VARIANT_TYPE_UINT,
			VARIANT_TYPE_UINT32,
			VARIANT_TYPE_UINT64,
			VARIANT_TYPE_INT,
			VARIANT_TYPE_INT32,
			VARIANT_TYPE_INT64,
			VARIANT_TYPE_FLOAT,
			VARIANT_TYPE_DOUBLE,
		};
		class TinyVisualVariant
		{
		public:
			TinyVisualVariant();
			virtual ~TinyVisualVariant();
			BOOL	IsEmpty() const;
			SIZE*	GetSize();
			RECT*	GetRect();
			HFONT	GetFont();
			CHAR*	GetString();
			BOOL	SetSize(const SIZE* s);
			BOOL	SetRect(const RECT* s);
			BOOL	SetFont(HFONT s);
			BOOL	SetString(const string& s);
			BOOL	SetString(LPCSTR s);
			void	Release();
		private:
			UINT			m_size;
			void*			m_value;
			VARIANT_TYPE	m_type;
		};
	}
}



