#pragma once
#include "TinyVisualCommon.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ͼƬ
		/// </summary>
		class ImageStyle : public TinyReference<ImageStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(ImageStyle)
		public:
			~ImageStyle();
			static TinyScopedReferencePtr<ImageStyle> Create();
			void SetImage(TinyImage* image);
			void SetCenter(const TinyRectangle& center);
			TinyImage*		GetImage() const;
			TinyRectangle	GetCenter() const;
		private:
			ImageStyle();
		private:
			TinyImage*		m_image;
			TinyRectangle	m_center;
		};
		/// <summary>
		/// ����
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
			ImageStyle*		GetImageStyle() const;
			COLORREF		GetColor() const;
			TinySize		GetSize() const;
			TinyPoint		GetPosition() const;
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
		/// �߿�
		/// </summary>
		class BorderStyle : public TinyReference<BorderStyle>
		{
		public:
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
			INT			GetThickness() const;
			COLORREF	GetColor() const;
			Style		GetStyle() const;
			ImageStyle*	GetImageStyle() const;
		private:
			BorderStyle();
		private:
			INT									m_thickness;
			Style								m_style;
			COLORREF							m_color;
			TinyScopedReferencePtr<ImageStyle>	m_imageStyle;
		};
		/// <summary>
		/// ����
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
			TinySize		GetSize() const;
			TinySize		GetMaximumSize() const;
			TinySize		GetMinimumSize() const;
			TinyRectangle	GetPadding() const;
			TinyRectangle	GetMargin() const;
			INT				GetZIndex() const;
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
		/// ���ӻ�
		/// </summary>
		class VisualStyle : public TinyReference<VisualStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(VisualStyle)
		public:
			~VisualStyle();
			static TinyScopedReferencePtr<VisualStyle> Create();
			void SetClip(HRGN clip);
			void SetZoom(FLOAT zoom);
			HRGN	GetClip() const;
			FLOAT	GetZoom() const;
		private:
			VisualStyle();
		private:
			HRGN			m_clip;
			FLOAT			m_zoom;
		};
		/// <summary>
		/// ����
		/// </summary>
		class FontStyle : public TinyReference<FontStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(FontStyle)
		public:
			~FontStyle();
			static TinyScopedReferencePtr<FontStyle> Create();
			void SetFont(HFONT hFONT);
		private:
			FontStyle();
		private:
			HFONT	m_hFONT;
		};
		/// <summary>
		/// ��Ⱦ
		/// </summary>
		class RenderStyle : public TinyReference<RenderStyle>
		{
			DISALLOW_COPY_AND_ASSIGN(RenderStyle)
		public:
			~RenderStyle();
			static TinyScopedReferencePtr<RenderStyle> Create();
			void	SetPadding(const TinyRectangle& padding);
			void	SetMargin(const TinyRectangle& margin);
			void	SetMaximumSize(const TinySize& size);
			void	SetMinimumSize(const TinySize& size);
			void	SetClip(HRGN hrgnClip);
			void	SetZIndex(INT zIndex);
			void	SetFont(HFONT hFONT);
			void	SetBackgroundImage(ImageStyle* image);
			void	SetBackgroundColor(COLORREF color);
			void	SetBackgroundPosition(const TinyPoint& pos);
			void	SetBackgroundSize(const TinySize& size);
			void	SetBorderColor(COLORREF color);
			void	SetBorderThickness(INT thickness);
			void	SetBorderStyle(BorderStyle::Style style);
			void	SetBorderImageStyle(ImageStyle* image);
			TinyRectangle		GetPadding() const;
			TinyRectangle		GetMargin() const;
			TinySize			GetMaximumSize() const;
			TinySize			GetMinimumSize() const;
			ImageStyle*			GetBackgroundImageStyle() const;
			TinySize			GetBackgroundSize() const;
			TinyPoint			GetBackgroundPosition() const;
			TinyColor			GetBackgroundColor() const;
			TinyColor			GetBorderColor() const;
			ImageStyle*			GetBorderImageStyle() const;

		protected:
			TinyScopedReferencePtr<BorderStyle>		m_borderStyle;
			TinyScopedReferencePtr<BoxStyle>		m_boxStyle;
			TinyScopedReferencePtr<VisualStyle>		m_visualStyle;
			TinyScopedReferencePtr<FontStyle>		m_fontStyle;
			TinyScopedReferencePtr<BackgroundStyle>	m_backgroundStyle;
		private:
			RenderStyle();
		};
	}
}


