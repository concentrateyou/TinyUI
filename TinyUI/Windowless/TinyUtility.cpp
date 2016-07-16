#include "../stdafx.h"
#include "TinyUtility.h"
#include "TinyVisualDocument.h"

namespace TinyUI
{
	namespace Windowless
	{
		LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
		{
			return (LONG)MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
		}
		LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
		{
			return (LONG)MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
		}
		LONG DXtoHimetricX(LONG dx, LONG xPerInch)
		{
			return (LONG)MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
		}
		LONG DYtoHimetricY(LONG dy, LONG yPerInch)
		{
			return (LONG)MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
		}
		//////////////////////////////////////////////////////////////////////////
		Vector2F::Vector2F()
			:m_x(0.0),
			m_y(0.0)
		{
		}
		Vector2F::Vector2F(FLOAT x, FLOAT y)
			: m_x(x),
			m_y(y)
		{
		}
		FLOAT Vector2F::X() const
		{
			return m_x;
		}
		FLOAT Vector2F::Y() const
		{
			return m_y;
		}
		void Vector2F::SetX(FLOAT x)
		{
			m_x = x;
		}
		void Vector2F::SetY(FLOAT y)
		{
			m_y = y;
		}
		BOOL Vector2F::IsZero() const
		{
			return m_x == 0 && m_y == 0;
		}
		void Vector2F::Add(const Vector2F& other)
		{
			m_x += other.m_x;
			m_y += other.m_y;
		}
		void Vector2F::Subtract(const Vector2F& other)
		{
			m_x -= other.m_x;
			m_y -= other.m_y;
		}
		void Vector2F::operator += (const Vector2F& other)
		{
			Add(other);
		}
		void Vector2F::operator -= (const Vector2F& other)
		{
			Subtract(other);
		}
		void Vector2F::SetMin(const Vector2F& other)
		{
			m_x = m_x <= other.m_x ? m_x : other.m_x;
			m_y = m_y <= other.m_y ? m_y : other.m_y;
		}
		void Vector2F::SetMax(const Vector2F& other)
		{
			m_x = m_x >= other.m_x ? m_x : other.m_x;
			m_y = m_y >= other.m_y ? m_y : other.m_y;
		}
		DOUBLE Vector2F::LengthSquared() const
		{
			return static_cast<double>(m_x)* m_x + static_cast<double>(m_y)* m_y;
		}
		FLOAT Vector2F::Length() const
		{
			return static_cast<float>(std::sqrt(LengthSquared()));
		}
		void Vector2F::Scale(DOUBLE scale)
		{
			Scale(scale, scale);
		}
		void Vector2F::Scale(DOUBLE x_scale, DOUBLE y_scale)
		{
			m_x *= static_cast<FLOAT>(x_scale);
			m_y *= static_cast<FLOAT>(y_scale);;
		}
		TinyString Vector2F::ToString() const
		{
			return TinyString::Format("[%f %f]", m_x, m_y);
		}
		//////////////////////////////////////////////////////////////////////////
		Vector3F::Vector3F()
			:m_x(0),
			m_y(0),
			m_z(0)
		{

		}
		Vector3F::Vector3F(FLOAT x, FLOAT y, FLOAT z)
			:m_x(x),
			m_y(y),
			m_z(z)
		{

		}
		Vector3F::Vector3F(const Vector2F& other)
			:m_x(other.X()),
			m_y(other.Y()),
			m_z(0)
		{

		}
		FLOAT Vector3F::X() const
		{
			return m_x;
		}
		void Vector3F::SetX(FLOAT x)
		{
			m_x = x;
		}
		FLOAT Vector3F::Y() const
		{
			return m_y;
		}
		void Vector3F::SetY(FLOAT y)
		{
			m_y = y;
		}
		FLOAT Vector3F::Z() const
		{
			return m_z;
		}
		void Vector3F::SetZ(FLOAT z)
		{
			m_z = z;
		}
		BOOL Vector3F::IsZero() const
		{
			return m_x == 0 && m_y == 0 && m_z == 0;
		}
		void Vector3F::Add(const Vector3F& other)
		{
			m_x += other.m_x;
			m_y += other.m_y;
			m_z += other.m_z;
		}
		void Vector3F::Subtract(const Vector3F& other)
		{
			m_x -= other.m_x;
			m_y -= other.m_y;
			m_z -= other.m_z;
		}
		void Vector3F::operator += (const Vector3F& other)
		{
			Add(other);
		}
		void Vector3F::operator -= (const Vector3F& other)
		{
			Subtract(other);
		}
		void Vector3F::SetMin(const Vector3F& other)
		{
			m_x = m_x <= other.m_x ? m_x : other.m_x;
			m_y = m_y <= other.m_y ? m_y : other.m_y;
			m_z = m_z <= other.m_z ? m_z : other.m_z;
		}
		void Vector3F::SetMax(const Vector3F& other)
		{
			m_x = m_x >= other.m_x ? m_x : other.m_x;
			m_y = m_y >= other.m_y ? m_y : other.m_y;
			m_z = m_z >= other.m_z ? m_z : other.m_z;
		}
		DOUBLE Vector3F::LengthSquared() const
		{
			return static_cast<double>(m_x)* m_x + static_cast<double>(m_y)* m_y + static_cast<double>(m_z)* m_z;
		}
		FLOAT Vector3F::Length() const
		{
			return static_cast<float>(std::sqrt(LengthSquared()));
		}
		void Vector3F::Scale(FLOAT scale)
		{
			Scale(scale, scale, scale);
		}
		void Vector3F::Scale(FLOAT x_scale, FLOAT y_scale, FLOAT z_scale)
		{
			m_x *= x_scale;
			m_y *= y_scale;
			m_z *= z_scale;
		}
		void Vector3F::Cross(const Vector3F& other)
		{
			FLOAT x = m_y * other.Z() - m_z * other.Y();
			FLOAT y = m_z * other.X() - m_x * other.Z();
			FLOAT z = m_x * other.Y() - m_y * other.X();
			m_x = x;
			m_y = y;
			m_z = z;
		}
		TinyString Vector3F::ToString() const
		{
			return TinyString::Format("[%f %f %f]", m_x, m_y, m_z);
		}
		//////////////////////////////////////////////////////////////////////////
		TinyClipCanvas::TinyClipCanvas(HDC hDC, TinyVisual* spvis, const RECT& rcPaint)
			:TinyCanvas(hDC)
		{
			ASSERT(spvis);
			m_hRGN = ::CreateRectRgnIndirect(&rcPaint);
			if (spvis->GetClip())
			{
				::CombineRgn(m_hRGN, m_hRGN, spvis->GetClip(), RGN_AND);
			}
			::ExtSelectClipRgn(m_hDC, m_hRGN, RGN_AND);
		}
		TinyClipCanvas::~TinyClipCanvas()
		{
			SAFE_DELETE_OBJECT(m_hRGN);
		}
	}
}
