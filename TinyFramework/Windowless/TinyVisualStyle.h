#pragma once
#include "TinyVisualCommon.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// ±³¾°
		/// </summary>
		class BackgroundStyle
		{
			DISALLOW_COPY_AND_ASSIGN(BackgroundStyle)
		public:
			BackgroundStyle();
			~BackgroundStyle();
			void			SetColor(COLORREF color);//±³¾°ÑÕÉ«
			void			SetPosition(const TinyPoint& pos);//±³¾°Í¼ÏñµÄÎ»ÖÃ
			void			SetSize(const TinySize& size);//±³¾°Í¼Æ¬µÄ³ß´ç
			void			SetCenter(const TinyRectangle& center);//±³¾°Í¼Æ¬¾Å¹¬¸ñÖÐ¼äËõ·Å´óÐ¡ 
			void			SetImage(TinyImage* image);
			COLORREF		GetColor() const;
			TinySize		GetSize() const;
			TinyPoint		GetPosition() const;
			TinyRectangle	GetCenter() const;
			TinyImage*		GetImage();
		private:
			COLORREF							m_color;
			TinyImage*							m_image;
			TinyPoint							m_position;
			TinySize							m_size;
			TinyRectangle						m_center;
		};
		/// <summary>
		/// ±ß¿ò
		/// </summary>
		class BorderStyle
		{
		public:
			enum Style
			{
				NONE, DOTTED, DASHED, SOLID, DOUBLE
			};
			DISALLOW_COPY_AND_ASSIGN(BorderStyle)
		public:
			BorderStyle();
			~BorderStyle();
			void			SetColor(COLORREF color);
			void			SetThickness(UINT thickness);
			void			SetStyle(Style style);
			void			SetImage(TinyImage* image);
			void			SetCenter(const TinyRectangle& center);
			UINT			GetThickness() const;
			COLORREF		GetColor() const;
			Style			GetStyle() const;
			TinyRectangle	GetCenter() const;
			TinyImage*		GetImage();
		private:
			UINT					m_thickness;
			COLORREF				m_color;
			Style					m_style;
			TinyRectangle			m_center;
			TinyImage*				m_image;
		};
		/// <summary>
		/// ºÐ×Ó
		/// </summary>
		class BoxStyle
		{
			DISALLOW_COPY_AND_ASSIGN(BoxStyle)
		public:
			BoxStyle();
			~BoxStyle();
			void			SetSize(const TinySize& size);
			void			SetMaximumSize(const TinySize& maximumSize);
			void			SetMinimumSize(const TinySize& minimumSize);
			void			SetPadding(const TinyRectangle& padding);
			void			SetMargin(const TinyRectangle& margin);
			void			SetZIndex(INT zIndex);
			TinySize		GetSize() const;
			TinySize		GetMaximumSize() const;
			TinySize		GetMinimumSize() const;
			TinyRectangle	GetPadding() const;
			TinyRectangle	GetMargin() const;
			INT				GetZIndex() const;
		private:
			INT				m_zIndex;
			TinySize		m_size;
			TinySize		m_maximumSize;
			TinySize		m_minimumSize;
			TinyRectangle	m_padding;
			TinyRectangle	m_margin;
		};
		/// <summary>
		/// ¿ÉÊÓ»¯
		/// </summary>
		class VisualStyle
		{
			DISALLOW_COPY_AND_ASSIGN(VisualStyle)
		public:
			VisualStyle();
			~VisualStyle();
			void	SetClip(HRGN clip);
			void	SetZoom(FLOAT zoom);
			HRGN	GetClip() const;
			FLOAT	GetZoom() const;
		private:
			HRGN			m_clip;
			FLOAT			m_zoom;
		};
		/// <summary>
		/// ×ÖÌå
		/// </summary>
		class FontStyle
		{
			DISALLOW_COPY_AND_ASSIGN(FontStyle)
		public:
			FontStyle();
			~FontStyle();
			void	SetFont(HFONT hFONT);
			HFONT	GetFont() const;
		private:
			HFONT	m_hFONT;
		};
	}
}



