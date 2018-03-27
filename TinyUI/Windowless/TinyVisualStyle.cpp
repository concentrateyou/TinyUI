#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualStyle.h"

namespace TinyUI
{
	namespace Windowless
	{
		ImageStyle::ImageStyle()
			:m_image(NULL)
		{

		}
		ImageStyle::~ImageStyle()
		{

		}
		TinyScopedReferencePtr<ImageStyle> ImageStyle::Create()
		{
			return (new ImageStyle());
		}
		void ImageStyle::SetImage(TinyImage* image)
		{
			m_image = image;
		}
		void ImageStyle::SetCenter(const TinyRectangle& center)
		{
			m_center = center;
		}
		TinyImage* ImageStyle::GetImage() const
		{
			return m_image;
		}
		TinyRectangle ImageStyle::GetCenter() const
		{
			return m_center;
		}
		//////////////////////////////////////////////////////////////////////////
		BackgroundStyle::BackgroundStyle()
			:m_color(0)
		{

		}
		BackgroundStyle::~BackgroundStyle()
		{

		}
		TinyScopedReferencePtr<BackgroundStyle> BackgroundStyle::Create()
		{
			return (new BackgroundStyle());
		}
		void BackgroundStyle::SetColor(COLORREF color)
		{
			m_color = color;
		}
		void BackgroundStyle::SetPosition(const TinyPoint& pos)
		{
			m_position = pos;
		}
		void BackgroundStyle::SetSize(const TinySize& size)
		{
			m_size = size;
		}
		void BackgroundStyle::SetImageStyle(ImageStyle* imageStyle)
		{
			m_imageStyle = imageStyle;
		}
		COLORREF BackgroundStyle::GetColor() const
		{
			return m_color;
		}
		TinySize BackgroundStyle::GetSize() const
		{
			return m_size;
		}
		TinyPoint BackgroundStyle::GetPosition() const
		{
			return m_position;
		}
		ImageStyle*	BackgroundStyle::GetImageStyle() const
		{
			return m_imageStyle;
		}
		//////////////////////////////////////////////////////////////////////////
		BorderStyle::BorderStyle()
			:m_color(0),
			m_style(NONE)
		{

		}
		BorderStyle::~BorderStyle()
		{

		}
		TinyScopedReferencePtr<BorderStyle> BorderStyle::Create()
		{
			return (new BorderStyle());
		}
		void BorderStyle::SetColor(COLORREF color)
		{
			m_color = color;
		}
		void BorderStyle::SetThickness(INT thickness)
		{
			m_thickness = thickness;
		}
		void BorderStyle::SetStyle(Style style)
		{
			m_style = style;
		}
		void BorderStyle::SetImageStyle(ImageStyle* imageStyle)
		{
			m_imageStyle = imageStyle;
		}
		COLORREF BorderStyle::GetColor() const
		{
			return m_color;
		}
		INT	BorderStyle::GetThickness() const
		{
			return m_thickness;
		}
		BorderStyle::Style BorderStyle::GetStyle() const
		{
			return m_style;
		}
		ImageStyle*	BorderStyle::GetImageStyle() const
		{
			return m_imageStyle;
		}
		//////////////////////////////////////////////////////////////////////////
		BoxStyle::BoxStyle()
			:m_zIndex(INT_MAX)
		{

		}
		BoxStyle::~BoxStyle()
		{

		}
		TinyScopedReferencePtr<BoxStyle> BoxStyle::Create()
		{
			return (new BoxStyle());
		}
		void BoxStyle::SetSize(const TinySize& size)
		{
			m_size = size;
		}
		void BoxStyle::SetMaximumSize(const TinySize& maximumSize)
		{
			m_maximumSize = maximumSize;
		}
		void BoxStyle::SetMinimumSize(const TinySize& minimumSize)
		{
			m_minimumSize = minimumSize;
		}
		void BoxStyle::SetZIndex(INT zIndex)
		{
			m_zIndex = zIndex;
		}
		void BoxStyle::SetPadding(const TinyRectangle& padding)
		{
			m_padding = padding;
		}
		void BoxStyle::SetMargin(const TinyRectangle& margin)
		{
			m_margin = margin;
		}
		TinySize BoxStyle::GetSize() const
		{
			return m_size;
		}
		TinySize BoxStyle::GetMaximumSize() const
		{
			return m_maximumSize;
		}
		TinySize BoxStyle::GetMinimumSize() const
		{
			return m_minimumSize;
		}
		TinyRectangle BoxStyle::GetPadding() const
		{
			return m_padding;
		}
		TinyRectangle BoxStyle::GetMargin() const
		{
			return m_margin;
		}
		INT	BoxStyle::GetZIndex() const
		{
			return m_zIndex;
		}
		//////////////////////////////////////////////////////////////////////////
		VisualStyle::VisualStyle()
			:m_zoom(1.0F)
		{

		}
		VisualStyle::~VisualStyle()
		{
			SAFE_DELETE_OBJECT(m_clip);
		}
		TinyScopedReferencePtr<VisualStyle> VisualStyle::Create()
		{
			return (new VisualStyle());
		}
		void VisualStyle::SetClip(HRGN clip)
		{
			m_clip = clip;
		}
		void VisualStyle::SetZoom(FLOAT zoom)
		{
			m_zoom = zoom;
		}
		HRGN VisualStyle::GetClip() const
		{
			return m_clip;
		}
		FLOAT VisualStyle::GetZoom() const
		{
			return m_zoom;
		}
		//////////////////////////////////////////////////////////////////////////
		FontStyle::FontStyle()
			:m_hFONT(NULL)
		{

		}
		FontStyle::~FontStyle()
		{
			SAFE_DELETE_OBJECT(m_hFONT);
		}
		TinyScopedReferencePtr<FontStyle> FontStyle::Create()
		{
			return (new FontStyle());
		}
		void FontStyle::SetFont(HFONT hFONT)
		{
			m_hFONT = hFONT;
		}
		//////////////////////////////////////////////////////////////////////////
		RenderStyle::RenderStyle()
		{

		}
		RenderStyle::~RenderStyle()
		{

		}
		void RenderStyle::SetPadding(const TinyRectangle& padding)
		{
			if (m_boxStyle != NULL)
			{
				m_boxStyle->SetPadding(padding);
			}
		}
		void RenderStyle::SetMargin(const TinyRectangle& margin)
		{
			if (m_boxStyle != NULL)
			{
				m_boxStyle->SetMargin(margin);
			}
		}
		void RenderStyle::SetMaximumSize(const TinySize& size)
		{
			if (m_boxStyle != NULL)
			{
				m_boxStyle->SetMaximumSize(size);
			}
		}
		void RenderStyle::SetMinimumSize(const TinySize& size)
		{
			if (m_boxStyle != NULL)
			{
				m_boxStyle->SetMinimumSize(size);
			}
		}
		void RenderStyle::SetClip(HRGN hrgnClip)
		{
			if (m_visualStyle != NULL)
			{
				m_visualStyle->SetClip(hrgnClip);
			}
		}
		void RenderStyle::SetZIndex(INT zIndex)
		{
			if (m_boxStyle != NULL)
			{
				m_boxStyle->SetZIndex(zIndex);
			}
		}
		void RenderStyle::SetFont(HFONT hFONT)
		{
			if (m_fontStyle != NULL)
			{
				m_fontStyle->SetFont(hFONT);
			}
		}
		void RenderStyle::SetBackgroundColor(COLORREF color)
		{
			if (m_backgroundStyle != NULL)
			{
				m_backgroundStyle->SetColor(color);
			}
		}
		void RenderStyle::SetBackgroundPosition(const TinyPoint& pos)
		{
			if (m_backgroundStyle != NULL)
			{
				m_backgroundStyle->SetPosition(pos);
			}
		}
		void RenderStyle::SetBackgroundSize(const TinySize& size)
		{
			if (m_backgroundStyle != NULL)
			{
				m_backgroundStyle->SetSize(size);
			}
		}
		void RenderStyle::SetBackgroundImage(ImageStyle* image)
		{
			if (m_backgroundStyle != NULL)
			{
				m_backgroundStyle->SetImageStyle(image);
			}
		}
		void RenderStyle::SetBorderColor(COLORREF color)
		{
			if (m_borderStyle != NULL)
			{
				m_borderStyle->SetColor(color);
			}
		}
		void RenderStyle::SetBorderThickness(INT thickness)
		{
			if (m_borderStyle != NULL)
			{
				m_borderStyle->SetThickness(thickness);
			}
		}
		void RenderStyle::SetBorderStyle(BorderStyle::Style style)
		{
			if (m_borderStyle != NULL)
			{
				m_borderStyle->SetStyle(style);
			}
		}
		void RenderStyle::SetBorderImageStyle(ImageStyle* image)
		{
			if (m_borderStyle != NULL)
			{
				m_borderStyle->SetImageStyle(image);
			}
		}

		TinyRectangle RenderStyle::GetPadding() const
		{
			return m_boxStyle->GetPadding();
		}
		TinyRectangle RenderStyle::GetMargin() const
		{
			return m_boxStyle->GetMargin();
		}
		TinySize RenderStyle::GetMaximumSize() const
		{
			return m_boxStyle->GetMaximumSize();
		}
		TinySize RenderStyle::GetMinimumSize() const
		{
			return m_boxStyle->GetMinimumSize();
		}
		ImageStyle*	RenderStyle::GetBackgroundImageStyle() const
		{
			return m_backgroundStyle->GetImageStyle();
		}
		TinySize	RenderStyle::GetBackgroundSize() const
		{
			ASSERT(m_backgroundStyle);
			return m_backgroundStyle->GetSize();
		}
		TinyPoint	RenderStyle::GetBackgroundPosition() const
		{
			ASSERT(m_backgroundStyle);
			return m_backgroundStyle->GetPosition();
		}
		TinyColor	RenderStyle::GetBackgroundColor() const
		{
			ASSERT(m_backgroundStyle);
			return m_backgroundStyle->GetColor();
		}
		TinyColor	RenderStyle::GetBorderColor() const
		{
			ASSERT(m_backgroundStyle);
			return m_backgroundStyle->GetColor();
		}
		ImageStyle*	RenderStyle::GetBorderImageStyle() const
		{
			return m_borderStyle->GetImageStyle();
		}
		TinyScopedReferencePtr<RenderStyle> RenderStyle::Create()
		{
			RenderStyle* style = new RenderStyle();
			if (style != NULL)
			{
				style->m_fontStyle = FontStyle::Create();
				style->m_backgroundStyle = BackgroundStyle::Create();
				style->m_boxStyle = BoxStyle::Create();
				style->m_visualStyle = VisualStyle::Create();
				style->m_borderStyle = BorderStyle::Create();
			}
			return style;
		}
	}
}
