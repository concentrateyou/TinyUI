#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
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
			static const TinyString SIZE;
			static const TinyString VISIBLE;
			static const TinyString ENABLE;
			static const TinyString IMAGENORMAL;
			static const TinyString IMAGEHIGHLIGHT;
			static const TinyString IMAGEDOWN;
		};
		/// <summary>
		/// Tag常量
		/// </summary>
		class TinyVisualTag
		{
		public:
			static const TinyString WINDOW;
			static const TinyString CAPTION;
			static const TinyString SYSBUTTON;
		};
		/// <summary>
		/// 缓存DC
		/// </summary>
		class TinyVisualCacheDC : public TinyHandleHDC
		{
		public:
			TinyVisualCacheDC(HWND hWND);
			~TinyVisualCacheDC();
			void SetSize(INT cx, INT cy);
		protected:
			HWND		m_hWND;
			TinySize	m_size;
			HBITMAP		m_hBitmap;
			HGDIOBJ		m_hPrev;
		};
	}
}



