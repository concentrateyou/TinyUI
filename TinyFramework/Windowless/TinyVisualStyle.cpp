#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualStyle.h"

namespace TinyFramework
{
	namespace Windowless
	{
		BackgroundStyle::BackgroundStyle()
			:m_color(0),
			m_image(NULL)
		{

		}
		BackgroundStyle::~BackgroundStyle()
		{

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
		void BackgroundStyle::SetCenter(const TinyRectangle& center)
		{
			m_center = center;
		}
		void BackgroundStyle::SetImage(TinyImage* image)
		{
			m_image = image;
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
		TinyRectangle BackgroundStyle::GetCenter() const
		{
			return m_center;
		}
		TinyImage* BackgroundStyle::GetImage()
		{
			return m_image;
		}
		//////////////////////////////////////////////////////////////////////////
		BorderStyle::BorderStyle()
			:m_color(0),
			m_style(NONE),
			m_thickness(0),
			m_image(NULL)
		{

		}
		BorderStyle::~BorderStyle()
		{

		}
		void BorderStyle::SetColor(COLORREF color)
		{
			m_color = color;
		}
		void BorderStyle::SetThickness(UINT thickness)
		{
			m_thickness = thickness;
		}
		void BorderStyle::SetStyle(Style style)
		{
			m_style = style;
		}
		void BorderStyle::SetImage(TinyImage* image)
		{
			m_image = image;
		}
		void BorderStyle::SetCenter(const TinyRectangle& center)
		{
			m_center = center;
		}
		COLORREF BorderStyle::GetColor() const
		{
			return m_color;
		}
		BorderStyle::Style BorderStyle::GetStyle() const
		{
			return m_style;
		}
		UINT BorderStyle::GetThickness() const
		{
			return m_thickness;
		}
		TinyRectangle BorderStyle::GetCenter() const
		{
			return m_center;
		}
		TinyImage*	 BorderStyle::GetImage()
		{
			return m_image;
		}
		//////////////////////////////////////////////////////////////////////////
		BoxStyle::BoxStyle()
			:m_zIndex(INT_MAX)
		{

		}
		BoxStyle::~BoxStyle()
		{

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
		void FontStyle::SetFont(HFONT hFONT)
		{
			m_hFONT = hFONT;
		}
		HFONT FontStyle::GetFont() const
		{
			return m_hFONT;
		}
		BOOL FontStyle::IsEmpty() const
		{
			return m_hFONT == NULL;
		}
	}
}
