#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
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
	}
}



