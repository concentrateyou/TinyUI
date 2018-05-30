#include "../stdafx.h"
#include "TinyRenderTarget.h"

namespace TinyFramework
{
	TinyD2DPointF::TinyD2DPointF(const TinyPoint& pt)
	{
		x = (FLOAT)pt.x;
		y = (FLOAT)pt.y;
	}

	TinyD2DPointF::TinyD2DPointF(const D2D1_POINT_2F& pt)
	{
		x = pt.x;
		y = pt.y;
	}

	TinyD2DPointF::TinyD2DPointF(const D2D1_POINT_2F* pt)
	{
		x = pt == NULL ? 0 : pt->x;
		y = pt == NULL ? 0 : pt->y;
	}

	TinyD2DPointF::TinyD2DPointF(FLOAT fX, FLOAT fY)
	{
		x = fX;
		y = fY;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyD2DPointU::TinyD2DPointU(const TinyPoint& pt)
	{
		x = (UINT32)pt.x;
		y = (UINT32)pt.y;
	}

	TinyD2DPointU::TinyD2DPointU(const D2D1_POINT_2U& pt)
	{
		x = pt.x;
		y = pt.y;
	}

	TinyD2DPointU::TinyD2DPointU(const D2D1_POINT_2U* pt)
	{
		x = pt == NULL ? 0 : pt->x;
		y = pt == NULL ? 0 : pt->y;
	}

	TinyD2DPointU::TinyD2DPointU(UINT32 uX, UINT32 uY)
	{
		x = uX;
		y = uY;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyD2DSizeF::TinyD2DSizeF(const TinySize& size)
	{
		width = (FLOAT)size.cx;
		height = (FLOAT)size.cy;
	}

	TinyD2DSizeF::TinyD2DSizeF(const D2D1_SIZE_F& size)
	{
		width = size.width;
		height = size.height;
	}

	TinyD2DSizeF::TinyD2DSizeF(const D2D1_SIZE_F* size)
	{
		width = size == NULL ? 0 : size->width;
		height = size == NULL ? 0 : size->height;
	}

	TinyD2DSizeF::TinyD2DSizeF(FLOAT cx, FLOAT cy)
	{
		width = cx;
		height = cy;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyD2DSizeU::TinyD2DSizeU(const TinySize& size)
	{
		width = (UINT32)size.cx;
		height = (UINT32)size.cy;
	}

	TinyD2DSizeU::TinyD2DSizeU(const D2D1_SIZE_U& size)
	{
		width = size.width;
		height = size.height;
	}

	TinyD2DSizeU::TinyD2DSizeU(const D2D1_SIZE_U* size)
	{
		width = size == NULL ? 0 : size->width;
		height = size == NULL ? 0 : size->height;
	}

	TinyD2DSizeU::TinyD2DSizeU(UINT32 cx, UINT32 cy)
	{
		width = cx;
		height = cy;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyD2DRectF::TinyD2DRectF(const TinyRectangle& rect)
	{
		left = (FLOAT)rect.left;
		right = (FLOAT)rect.right;
		top = (FLOAT)rect.top;
		bottom = (FLOAT)rect.bottom;
	}

	TinyD2DRectF::TinyD2DRectF(const D2D1_RECT_F& rect)
	{
		left = rect.left;
		right = rect.right;
		top = rect.top;
		bottom = rect.bottom;
	}

	TinyD2DRectF::TinyD2DRectF(const D2D1_RECT_F* rect)
	{
		left = rect == NULL ? 0 : rect->left;
		right = rect == NULL ? 0 : rect->right;
		top = rect == NULL ? 0 : rect->top;
		bottom = rect == NULL ? 0 : rect->bottom;
	}

	TinyD2DRectF::TinyD2DRectF(FLOAT fLeft, FLOAT fTop, FLOAT fRight, FLOAT fBottom)
	{
		left = fLeft;
		right = fRight;
		top = fTop;
		bottom = fBottom;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyD2DRectU::TinyD2DRectU(const TinyRectangle& rect)
	{
		left = (UINT32)rect.left;
		right = (UINT32)rect.right;
		top = (UINT32)rect.top;
		bottom = (UINT32)rect.bottom;
	}

	TinyD2DRectU::TinyD2DRectU(const D2D1_RECT_U& rect)
	{
		left = rect.left;
		right = rect.right;
		top = rect.top;
		bottom = rect.bottom;
	}

	TinyD2DRectU::TinyD2DRectU(const D2D1_RECT_U* rect)
	{
		left = rect == NULL ? 0 : rect->left;
		right = rect == NULL ? 0 : rect->right;
		top = rect == NULL ? 0 : rect->top;
		bottom = rect == NULL ? 0 : rect->bottom;
	}

	TinyD2DRectU::TinyD2DRectU(UINT32 uLeft, UINT32 uTop, UINT32 uRight, UINT32 uBottom)
	{
		left = uLeft;
		right = uRight;
		top = uTop;
		bottom = uBottom;
	}

	TinyD2DRoundedRect::TinyD2DRoundedRect(const TinyD2DRectF& rectIn, const TinyD2DSizeF& sizeRadius)
	{
		rect = rectIn;
		radiusX = sizeRadius.width;
		radiusY = sizeRadius.height;
	}

	TinyD2DRoundedRect::TinyD2DRoundedRect(const D2D1_ROUNDED_RECT& rectIn)
	{
		rect = rectIn.rect;
		radiusX = rectIn.radiusX;
		radiusY = rectIn.radiusY;
	}

	TinyD2DRoundedRect::TinyD2DRoundedRect(const D2D1_ROUNDED_RECT* rectIn)
	{
		if (rectIn != NULL)
		{
			rect = rectIn->rect;
			radiusX = rectIn->radiusX;
			radiusY = rectIn->radiusY;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TinyD2DEllipse::TinyD2DEllipse(const TinyD2DRectF& rect)
	{
		radiusX = .5f * (rect.right - rect.left);
		radiusY = .5f * (rect.bottom - rect.top);
		point.x = rect.left + radiusX;
		point.y = rect.top + radiusY;
	}

	TinyD2DEllipse::TinyD2DEllipse(const D2D1_ELLIPSE& ellipse)
	{
		point = ellipse.point;
		radiusX = ellipse.radiusX;
		radiusY = ellipse.radiusY;
	}

	TinyD2DEllipse::TinyD2DEllipse(const D2D1_ELLIPSE* ellipse)
	{
		if (ellipse == NULL)
		{
			point.x = point.y = 0;
			radiusX = 0.0f;
			radiusY = 0.0f;
		}
		else
		{
			point = ellipse->point;
			radiusX = ellipse->radiusX;
			radiusY = ellipse->radiusY;
		}
	}

	TinyD2DEllipse::TinyD2DEllipse(const TinyD2DPointF& ptCenter, const TinyD2DSizeF& sizeRadius)
	{
		point.x = ptCenter.x;
		point.y = ptCenter.y;
		radiusX = sizeRadius.width;
		radiusY = sizeRadius.height;
	}
};