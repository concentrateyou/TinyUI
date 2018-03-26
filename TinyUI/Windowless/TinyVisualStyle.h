#pragma once
#include "TinyVisualCommon.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// Õº∆¨
		/// </summary>
		class ImageStyle : public TinyReference<ImageStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(ImageStyle)
		public:
			~ImageStyle();
			static TinyScopedReferencePtr<ImageStyle> Create();
			void SetImage(TinyImage* image);
			void SetCenter(const TinyRectangle& center);
			TinyImage*		GetImage();
			TinyRectangle	GetCenter();
		private:
			ImageStyle();
		private:
			TinyImage*		m_image;
			TinyRectangle	m_center;
		};
		/// <summary>
		/// ±≥æ∞
		/// </summary>
		class BackgroundStyle : public TinyReference<BackgroundStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(BackgroundStyle)
		public:
			~BackgroundStyle();
			static TinyScopedReferencePtr<BackgroundStyle> Create();
			void SetColor(COLORREF color);
			void SetPosition(const TinyPoint& pos);
			void SetSize(const TinySize& size);
			void SetImageStyle(ImageStyle* imageStyle);
			ImageStyle*		GetImageStyle();
			COLORREF		GetColor();
			TinySize		GetSize();
			TinyPoint		GetPosition();
		private:
			BackgroundStyle();
		private:
			COLORREF							m_color;
			TinyPoint							m_position;
			TinySize							m_size;
			TinyReference						m_center;
			TinyScopedReferencePtr<ImageStyle>	m_imageStyle;
		};
		/// <summary>
		/// ±ﬂøÚ
		/// </summary>
		class BorderStyle : public TinyReference<BorderStyle>
		{
			enum Style
			{
				NONE, DOTTED, DASHED, SOLID, DOUBLE
			};
			DISALLOW_COPY_AND_ASSIGN(BorderStyle)
		public:
			~BorderStyle();
			static TinyScopedReferencePtr<BorderStyle> Create();
			void SetColor(COLORREF color);
			void SetThickness(INT thickness);
			void SetStyle(Style style);
			void SetImageStyle(ImageStyle* imageStyle);
			INT			GetThickness();
			COLORREF	GetColor();
			Style		GetStyle();
		private:
			BorderStyle();
		private:
			INT									m_thickness;
			Style								m_style;
			COLORREF							m_color;
			TinyScopedReferencePtr<ImageStyle>	m_imageStyle;
		};
		/// <summary>
		/// ∫–◊”
		/// </summary>
		class BoxStyle : public TinyReference<BoxStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(BoxStyle)
		public:
			~BoxStyle();
			static TinyScopedReferencePtr<BoxStyle> Create();
			void SetSize(const TinySize& size);
			void SetMaximumSize(const TinySize& maximumSize);
			void SetMinimumSize(const TinySize& minimumSize);
			void SetPadding(const TinyRectangle& padding);
			void SetMargin(const TinyRectangle& margin);
			void SetZIndex(INT zIndex);
			TinySize		GetSize();
			TinySize		GetMaximumSize();
			TinySize		GetMinimumSize();
			TinyRectangle	GetPadding();
			TinyRectangle	GetMargin();
			INT				GetZIndex();
		private:
			BoxStyle();
		private:
			INT				m_zIndex;
			TinySize		m_size;
			TinySize		m_maximumSize;
			TinySize		m_minimumSize;
			TinyRectangle	m_padding;
			TinyRectangle	m_margin;
		};
		/// <summary>
		/// ø… ”ªØ
		/// </summary>
		class VisualStyle : public TinyReference<VisualStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(VisualStyle)
		public:
			~VisualStyle();
			static TinyScopedReferencePtr<VisualStyle> Create();
			void SetClip(HRGN clip);
			void SetZoom(FLOAT zoom);
		private:
			VisualStyle();
		private:
			HRGN			m_clip;
			FLOAT			m_zoom;
		};
		/// <summary>
		/// ‰÷»æ
		/// </summary>
		class RenderStyle : public TinyReference<RenderStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(RenderStyle)
		public:
			~RenderStyle();
			static TinyScopedReferencePtr<RenderStyle> Create();
		private:
			TinyScopedReferencePtr<BoxStyle>		m_boxStyle;
			TinyScopedReferencePtr<VisualStyle>		m_visualStyle;
			TinyScopedReferencePtr<BackgroundStyle>	m_backgroundStyle;
		private:
			RenderStyle();
		};
	}
}



