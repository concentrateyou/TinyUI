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
		TinyImage* ImageStyle::GetImage()
		{
			return m_image;
		}
		TinyRectangle ImageStyle::GetCenter()
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
		COLORREF BackgroundStyle::GetColor()
		{
			return m_color;
		}
		TinySize BackgroundStyle::GetSize()
		{
			return m_size;
		}
		TinyPoint BackgroundStyle::GetPosition()
		{
			return m_position;
		}
		ImageStyle*	BackgroundStyle::GetImageStyle()
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
		COLORREF BorderStyle::GetColor()
		{
			return m_color;
		}
		INT	BorderStyle::GetThickness()
		{
			return m_thickness;
		}
		BorderStyle::Style BorderStyle::GetStyle()
		{
			return m_style;
		}
		//////////////////////////////////////////////////////////////////////////
		BoxStyle::BoxStyle()
			:m_zIndex(0)
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
		TinySize BoxStyle::GetSize()
		{
			return m_size;
		}
		TinySize BoxStyle::GetMaximumSize()
		{
			return m_maximumSize;
		}
		TinySize BoxStyle::GetMinimumSize()
		{
			return m_minimumSize;
		}
		TinyRectangle BoxStyle::GetPadding()
		{
			return m_padding;
		}
		TinyRectangle BoxStyle::GetMargin()
		{
			return m_margin;
		}
		INT	BoxStyle::GetZIndex()
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
		//////////////////////////////////////////////////////////////////////////
		RenderStyle::RenderStyle()
		{

		}
		RenderStyle::~RenderStyle()
		{

		}
		TinyScopedReferencePtr<RenderStyle> RenderStyle::Create()
		{
			RenderStyle* style = new RenderStyle();
			if (style != NULL)
			{
				style->m_backgroundStyle = BackgroundStyle::Create();
				style->m_boxStyle = BoxStyle::Create();
				style->m_visualStyle = VisualStyle::Create();
			}
			return style;
		}
	}
}
