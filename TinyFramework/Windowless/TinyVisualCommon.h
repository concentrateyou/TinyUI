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

			static const TinyString LAYOUT;

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

		enum VisualLayout
		{
			None,
			Horizontal,
			Vertical
		};

		enum VARIANT_TYPE
		{
			VARIANT_TYPE_EMPTY,
			VARIANT_TYPE_RECT,
			VARIANT_TYPE_POINT,
			VARIANT_TYPE_SIZE,
			VARIANT_TYPE_FONT,
			VARIANT_TYPE_REGION,
			VARIANT_TYPE_STRING,
			VARIANT_TYPE_COLOR,
			VARIANT_TYPE_IMAGE,
			VARIANT_TYPE_BOOL,
			VARIANT_TYPE_INT32,
			VARIANT_TYPE_DOUBLE,
			VARIANT_TYPE_FLOAT
		};

		class TinyVisualVariant
		{
		public:
			TinyVisualVariant();
			TinyVisualVariant(TinyVisualVariant&& s);
			TinyVisualVariant(const TinyVisualVariant& s);
			TinyVisualVariant& operator=(const TinyVisualVariant& s);
			virtual ~TinyVisualVariant();
			BOOL		IsEmpty() const;
			BOOL		GetBool() const;
			INT32		GetInt32() const;
			DOUBLE		GetDouble() const;
			FLOAT		GetFloat() const;
			POINT		GetPoint() const;
			SIZE		GetSize() const;
			RECT		GetRect() const;
			HFONT		GetFont() const;
			const CHAR*	GetString() const;
			COLORREF	GetColor() const;
			TinyImage*	GetImage();
			BOOL		SetPoint(const POINT& s);
			BOOL		SetSize(const SIZE& s);
			BOOL		SetRect(const RECT& s);
			BOOL		SetFont(HFONT s);
			BOOL		SetRegion(HRGN s);
			BOOL		SetString(const string& s);
			BOOL		SetString(const TinyString& s);
			BOOL		SetString(const CHAR* s);
			BOOL		SetColor(COLORREF s);
			BOOL		SetImage(TinyImage&& s);
			BOOL		SetBool(BOOL s);
			BOOL		SetInt32(INT32 s);
			BOOL		SetDouble(DOUBLE s);
			BOOL		SetFloat(FLOAT s);
			void		Release();
		private:
			UINT			m_size;
			void*			m_value;
			VARIANT_TYPE	m_type;
		};

		template<class T>
		class TinyVisualVariantT
		{
		public:
			TinyVisualVariantT()
				:m_myP(NULL)
			{

			}
			~TinyVisualVariantT()
			{
				Release();
			}
			BOOL IsEmpty() const
			{
				return m_myP == NULL;
			}
			TinyVisualVariantT(const TinyVisualVariantT& myT)
			{
				T* myP = NULL;
				myP = (T*)malloc(sizeof(T));
				if (myP != NULL)
				{
#pragma push_macro("new")
#undef new
					::new(myP) TinyPlaceNew<T>(myT);
#pragma pop_macro("new")
					m_myP = myP;
				}
			}
			TinyVisualVariantT& operator=(const TinyVisualVariantT& myT)
			{
				if (&myT != this)
				{
					T* myP = NULL;
					myP = (T*)malloc(sizeof(T));
					if (myP != NULL)
					{
#pragma push_macro("new")
#undef new
						::new(myP) TinyPlaceNew<T>(myT);
#pragma pop_macro("new")
						Release();
						m_myP = myP;
					}
				}
				return *this;
			}
			TinyVisualVariantT(TinyVisualVariantT&& myT)
				:m_myP(myT.m_myP)
			{
				myT.m_myP = NULL;
			}
			void Release()
			{
				if (m_myP != NULL)
					m_myP->~T();
				SAFE_FREE(m_myP);
			}
		protected:
			T*	m_myP;
		};
	}
}



