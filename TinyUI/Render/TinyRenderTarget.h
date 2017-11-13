#pragma once
#include "../Common/TinyString.h"
#include "../Common/TinyEvent.h"
#include "../Render/TinyGDI.h"
#include <d2d1.h>

namespace TinyUI
{
	class TinyD2DPointF : public D2D1_POINT_2F
	{
	public:
		TinyD2DPointF(const TinyPoint& pt);
		TinyD2DPointF(const D2D1_POINT_2F& pt);
		TinyD2DPointF(const D2D1_POINT_2F* pt);
		TinyD2DPointF(FLOAT fX = 0., FLOAT fY = 0.);
		operator TinyPoint() { return TinyPoint((LONG)x, (LONG)y); }
	};

	class TinyD2DPointU : public D2D1_POINT_2U
	{
	public:
		TinyD2DPointU(const TinyPoint& pt);
		TinyD2DPointU(const D2D1_POINT_2U& pt);
		TinyD2DPointU(const D2D1_POINT_2U* pt);
		TinyD2DPointU(UINT32 uX = 0, UINT32 uY = 0);
		operator TinyPoint() { return TinyPoint((LONG)x, (LONG)y); }
	};

	class TinyD2DSizeF : public D2D1_SIZE_F
	{
	public:
		TinyD2DSizeF(const TinySize& size);
		TinyD2DSizeF(const D2D1_SIZE_F& size);
		TinyD2DSizeF(const D2D1_SIZE_F* size);
		TinyD2DSizeF(FLOAT cx = 0., FLOAT cy = 0.);
		BOOL IsNull() const { return width == 0. && height == 0.; }
		operator TinySize() { return TinySize((LONG)width, (LONG)height); }
	};

	class TinyD2DSizeU : public D2D1_SIZE_U
	{
	public:
		TinyD2DSizeU(const TinySize& size);
		TinyD2DSizeU(const D2D1_SIZE_U& size);
		TinyD2DSizeU(const D2D1_SIZE_U* size);
		TinyD2DSizeU(UINT32 cx = 0, UINT32 cy = 0);
		BOOL IsNull() const { return width == 0 && height == 0; }
		operator TinySize() { return TinySize((LONG)width, (LONG)height); }
	};

	class TinyD2DRectF : public D2D1_RECT_F
	{
	public:
		TinyD2DRectF(const TinyRectangle& rect);
		TinyD2DRectF(const D2D1_RECT_F& rect);
		TinyD2DRectF(const D2D1_RECT_F* rect);
		TinyD2DRectF(FLOAT fLeft = 0., FLOAT fTop = 0., FLOAT fRight = 0., FLOAT fBottom = 0.);
		BOOL IsNull() const { return left == 0. && right == 0. && top == 0. && bottom == 0.; }
		operator TinyRectangle() { return TinyRectangle((LONG)left, (LONG)top, (LONG)right, (LONG)bottom); }
	};

	class TinyD2DRectU : public D2D1_RECT_U
	{
	public:
		TinyD2DRectU(const TinyRectangle& rect);
		TinyD2DRectU(const D2D1_RECT_U& rect);
		TinyD2DRectU(const D2D1_RECT_U* rect);
		TinyD2DRectU(UINT32 uLeft = 0, UINT32 uTop = 0, UINT32 uRight = 0, UINT32 uBottom = 0);
		BOOL IsNull() const { return left == 0 && right == 0 && top == 0 && bottom == 0; }
		operator TinyRectangle() { return TinyRectangle((LONG)left, (LONG)top, (LONG)right, (LONG)bottom); }
	};

	class TinyD2DRoundedRect : public D2D1_ROUNDED_RECT
	{
	public:
		TinyD2DRoundedRect(const TinyD2DRectF& rectIn, const TinyD2DSizeF& sizeRadius);
		TinyD2DRoundedRect(const D2D1_ROUNDED_RECT& rectIn);
		TinyD2DRoundedRect(const D2D1_ROUNDED_RECT* rectIn);
	};

	class TinyD2DEllipse : public D2D1_ELLIPSE
	{
	public:
		TinyD2DEllipse(const TinyD2DRectF& rect);
		TinyD2DEllipse(const D2D1_ELLIPSE& ellipse);
		TinyD2DEllipse(const D2D1_ELLIPSE* ellipse);
		TinyD2DEllipse(const TinyD2DPointF& ptCenter, const TinyD2DSizeF& sizeRadius);
	};
};


