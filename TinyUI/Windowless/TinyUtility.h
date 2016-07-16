#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
#define LY_PER_INCH   1440
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
		LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch);
		LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch);
		LONG DXtoHimetricX(LONG dx, LONG xPerInch);
		LONG DYtoHimetricY(LONG dy, LONG yPerInch);
		/// <summary>
		/// 二维矩阵
		/// </summary>
		class Vector2F
		{
		public:
			Vector2F();
			Vector2F(FLOAT x, FLOAT y);
			FLOAT X() const;
			void SetX(FLOAT x);
			FLOAT Y() const;
			void SetY(FLOAT y);
			BOOL IsZero() const;
			void Add(const Vector2F& other);
			void Subtract(const Vector2F& other);
			void operator+=(const Vector2F& other);
			void operator-=(const Vector2F& other);
			void SetMin(const Vector2F& other);
			void SetMax(const Vector2F& other);
			DOUBLE LengthSquared() const;
			FLOAT Length() const;
			void Scale(DOUBLE scale);
			void Scale(DOUBLE x_scale, DOUBLE y_scale);
			TinyString ToString() const;
		private:
			FLOAT m_x;
			FLOAT m_y;
		};
		/// <summary>
		/// 三维矩阵
		/// </summary>
		class Vector3F
		{
		public:
			Vector3F();
			Vector3F(FLOAT x, FLOAT y, FLOAT z);
			explicit Vector3F(const Vector2F& other);
			FLOAT X() const;
			void SetX(FLOAT x);
			FLOAT Y() const;
			void SetY(FLOAT y);
			FLOAT Z() const;
			void SetZ(FLOAT z);
			BOOL IsZero() const;
			void Add(const Vector3F& other);
			void Subtract(const Vector3F& other);
			void operator+=(const Vector3F& other);
			void operator-=(const Vector3F& other);
			void SetMin(const Vector3F& other);
			void SetMax(const Vector3F& other);
			DOUBLE LengthSquared() const;
			FLOAT Length() const;
			void Scale(FLOAT scale);
			void Scale(FLOAT x_scale, FLOAT y_scale, FLOAT z_scale);
			void Cross(const Vector3F& other);
			TinyString ToString() const;
		private:
			FLOAT m_x;
			FLOAT m_y;
			FLOAT m_z;
		};
		/// <summary>
		/// 裁剪的DC在当前元素最小的矩形上绘制
		/// </summary>
		class TinyClipCanvas : public TinyCanvas
		{
		public:
			TinyClipCanvas(HDC hDC, TinyVisual* spvis, const RECT& rcPaint);
			virtual ~TinyClipCanvas();
		private:
			HRGN			m_hRGN;
		};
	}
}



