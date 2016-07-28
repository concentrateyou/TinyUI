#pragma once
#include <math.h>
#define M_PI            3.1415926535897932384626433832795f
#define RAD(val)        ((val)*0.0174532925199432957692369076848f)
#define DEG(val)        ((val)*57.295779513082320876798154814105f)
#define LARGE_EPSILON   1e-2f
#define EPSILON         1e-4f
#define TINY_EPSILON    1e-5f
#define M_INFINITE      3.4e38f
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

inline BOOL CloseFloat(float f1, float f2, float precision = LARGE_EPSILON)
{
	return fabsf(f1 - f2) <= precision;
}

namespace D3D
{
	struct  Vector2D
	{
		union
		{
			struct
			{
				float x;
				float y;
			};
			float ptr[2];
		};


		inline Vector2D()                                  {}
		inline Vector2D(const Vector2D &v) : x(v.x), y(v.y)   {}
		explicit inline Vector2D(float a) : x(a), y(a)     {}
		inline Vector2D(float a, float b) : x(a), y(b)     {}

		inline Vector2D operator+(const Vector2D &v) const    { return Vector2D(x + v.x, y + v.y); }
		inline Vector2D operator-(const Vector2D &v) const    { return Vector2D(x - v.x, y - v.y); }
		inline Vector2D operator*(const Vector2D &v) const    { return Vector2D(x*v.x, y*v.y); }
		inline Vector2D operator/(const Vector2D &v) const    { return Vector2D(x / v.x, y / v.y); }

		inline Vector2D operator+(float f) const           { return Vector2D(x + f, y + f); }
		inline Vector2D operator-(float f) const           { return Vector2D(x - f, y - f); }
		inline Vector2D operator*(float f) const           { return Vector2D(x*f, y*f); }
		inline Vector2D operator/(float f) const           { return Vector2D(x / f, y / f); }

		inline float operator|(const Vector2D &v) const    { return (x*v.x) + (y*v.y); }

		inline Vector2D& operator+=(const Vector2D &v)        { return (*this) = (*this) + v; }
		inline Vector2D& operator-=(const Vector2D &v)        { return (*this) = (*this) - v; }
		inline Vector2D& operator*=(const Vector2D &v)        { return (*this) = (*this)*v; }
		inline Vector2D& operator/=(const Vector2D &v)        { return (*this) = (*this) / v; }

		inline Vector2D& operator+=(float f)               { return (*this) = (*this) + f; }
		inline Vector2D& operator-=(float f)               { return (*this) = (*this) - f; }
		inline Vector2D& operator*=(float f)               { return (*this) = (*this)*f; }
		inline Vector2D& operator/=(float f)               { return (*this) = (*this) / f; }

		inline Vector2D& operator=(const Vector2D &v)         { x = v.x; y = v.y; return *this; }
		inline Vector2D& operator=(float val)              { x = val; y = val; return *this; }

		inline BOOL operator==(const Vector2D &v) const    { return (v.x == x) && (v.y == y); }
		inline BOOL operator!=(const Vector2D &v) const    { return (v.x != x) || (v.y != y); }

		inline Vector2D operator-() const                  { return Vector2D(-x, -y); }

		inline operator float*()                        { return ptr; }

		inline Vector2D& Set(float a, float b)             { x = a; y = b; return *this; }

		inline float Length() const                        { return (float)sqrt((double)(x*x) + (y*y)); }

		inline float Distance(const Vector2D &v) const
		{
			float a = (x - v.x), b = (y - v.y);
			return (float)sqrt((double)(a*a) + (b*b));
		}

		inline Vector2D& ClampMin(float val)      { if (x < val) x = val; if (y < val) y = val; return *this; }
		inline Vector2D& ClampMin(const Vector2D &v) { if (x < v.x) x = v.x; if (y < v.y) y = v.y; return *this; }

		inline Vector2D& ClampMax(float val)      { if (x > val) x = val; if (y > val) y = val; return *this; }
		inline Vector2D& ClampMax(const Vector2D &v) { if (x > v.x) x = v.x; if (y > v.y) y = v.y; return *this; }

		static inline Vector2D Max(const Vector2D &v, float val)       { return Vector2D(MAX(v.x, val), MAX(v.y, val)); }
		static inline Vector2D Max(const Vector2D &v1, const Vector2D &v2) { return Vector2D(MAX(v1.x, v2.x), MAX(v1.y, v2.y)); }

		static inline Vector2D Min(const Vector2D &v, float val)       { return Vector2D(MIN(v.x, val), MIN(v.y, val)); }
		static inline Vector2D Min(const Vector2D &v1, const Vector2D &v2) { return Vector2D(MIN(v1.x, v2.x), MIN(v1.y, v2.y)); }

		inline Vector2D& Abs()                             { x = fabs(x); y = fabs(y); return *this; }
		inline Vector2D GetAbs() const                     { return Vector2D(*this).Abs(); }

		inline Vector2D& Floor()                           { x = floorf(x); y = floorf(y); return *this; }
		inline Vector2D GetFloor() const                   { return Vector2D(*this).Floor(); }

		inline Vector2D& Ceil()                            { x = ceilf(x); y = ceilf(y); return *this; }
		inline Vector2D GetCeil() const                    { return Vector2D(*this).Ceil(); }

		inline BOOL CloseTo(const Vector2D &v, float epsilon = EPSILON) const
		{
			return CloseFloat(x, v.x, epsilon) && CloseFloat(y, v.y, epsilon);
		}

		inline Vector2D& Normal()
		{
			float len = Length();

			if (len != 0)
			{
				len = 1 / len;
				x *= len; y *= len;
			}

			return *this;
		}

		inline Vector2D GetNorm()
		{
			return Vector2D(*this).Normal();
		}

		inline float Dot(const Vector2D &v) const
		{
			return v | (*this);
		}

		inline Vector2D& Cross()
		{
			return (*this) = GetCross();
		}

		inline Vector2D GetCross() const
		{
			return Vector2D(y, -x);
		}

		inline Vector2D& SwapVals() { float newY = x; x = y; y = newY; return *this; }
	};

}


