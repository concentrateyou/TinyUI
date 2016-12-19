#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// Ԫ�����Գ���
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
			static const TinyString IMAGENORMAL;
			static const TinyString IMAGEHIGHLIGHT;
			static const TinyString IMAGEDOWN;
			static const TinyString FONTNAME;
			static const TinyString FONTSIZE;
			static const TinyString FONTCOLOR;
			static const TinyString FONTSTYLE;
		};
		/// <summary>
		/// Tag����
		/// </summary>
		class TinyVisualTag
		{
		public:
			static const TinyString WINDOW;
			static const TinyString CAPTION;
			static const TinyString LABEL;
			static const TinyString BUTTON;
			static const TinyString HORIZONTALLAYOUT;
			static const TinyString VERTICALLAYOUT;
			static const TinyString LISTBOX;
			static const TinyString TOOLBAR;
			static const TinyString TREEVIEW;
			static const TinyString HSCROLLBAR;
			static const TinyString VSCROLLBAR;
			static const TinyString COMBOBOX;
			static const TinyString RICHTEXT;
		};
		/// <summary>
		/// ����DC
		/// </summary>
		class TinyVisualCacheDC : public TinyHandleHDC
		{
		public:
			TinyVisualCacheDC(HWND hWND);
			~TinyVisualCacheDC();
			void SetSize(INT cx, INT cy);
			HDC	 GetMemDC() const;
			BOOL Render(const RECT& s);
			BOOL Render(const RECT& s, INT x, INT y);
		protected:
			HWND		m_hWND;
			TinySize	m_size;
			HDC			m_hMemDC;
			HBITMAP		m_hMemBitmap;
			HBITMAP		m_OldBitmap;
		};
	}
}



