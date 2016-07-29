#pragma once
#include <D3D10_1.h>
#include <D3DX10.h>
#include <DXGI.h>
#include <math.h>
#include "Common/TinyCommon.h"
#include "Common/TinyString.h"
namespace D3D
{
#define M_PI            3.1415926535897932384626433832795f
#define RAD(val)        ((val)*0.0174532925199432957692369076848f)
#define DEG(val)        ((val)*57.295779513082320876798154814105f)
#define LARGE_EPSILON   1e-2f
#define EPSILON         1e-4f
#define TINY_EPSILON    1e-5f
#define M_INFINITE      3.4e38f

	inline D3DXVECTOR2 Multiply(const D3DXVECTOR2& a1, const D3DXVECTOR2& a2)
	{
		return D3DXVECTOR2(a1.x*a2.x, a1.y*a2.y);
	}
	inline D3DXVECTOR2 Divide(const D3DXVECTOR2& a1, const D3DXVECTOR2& a2)
	{
		return D3DXVECTOR2(a1.x / a2.x, a1.y / a2.y);
	}

	inline void SetVector(D3DXVECTOR4& a, FLOAT x, FLOAT y, FLOAT z)
	{
		a.x = x;
		a.y = y;
		a.z = z;
		a.w = 0.0F;
	}
	inline void SetVector(D3DXVECTOR2& a, FLOAT x, FLOAT y)
	{
		a.x = x;
		a.y = y;
	}

	inline BOOL CloseFloat(FLOAT f1, FLOAT f2, float precision = LARGE_EPSILON)
	{
		return fabsf(f1 - f2) <= precision;
	}
	inline void Abs(D3DXVECTOR2& a)
	{
		a.x = fabs(a.x);
		a.y = fabs(a.y);
	}
}
