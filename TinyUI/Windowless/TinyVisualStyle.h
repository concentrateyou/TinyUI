#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class ImageStyle : public TinyReference<ImageStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(ImageStyle)
		public:
		private:
			TinyString	m_iamge;
		};
		/// <summary>
		/// ±³¾°
		/// </summary>
		class BackgroundStyle : public TinyReference<BackgroundStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(BackgroundStyle)
		public:

		private:
			COLORREF							m_color;
			TinyPoint							m_position;
			TinySize							m_size;
			TinyScopedReferencePtr<ImageStyle>	m_imageStyle;
		};
		/// <summary>
		/// ±ß¿ò
		/// </summary>
		class BorderStyle : public TinyReference<BorderStyle>
		{
			enum Style
			{
				NONE, DOTTED, DASHED, SOLID, DOUBLE
			};
			DISALLOW_COPY_AND_ASSIGN(BorderStyle)

		public:
		private:
			INT			m_size;
			COLORREF	m_color;
			Style		m_style;
		};
		/// <summary>
		/// ºÐ×Ó
		/// </summary>
		class BoxStyle : public TinyReference<BoxStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(BoxStyle)
		};
		/// <summary>
		/// äÖÈ¾
		/// </summary>
		class RenderStyle : public TinyReference<RenderStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(RenderStyle)
		public:
		private:
			TinyRectangle							m_padding;
			TinyRectangle							m_margin;
			TinyScopedReferencePtr<BorderStyle>		m_borderStyle;
			TinyScopedReferencePtr<BackgroundStyle>	m_backgroundStyle;
		};
	}
}



