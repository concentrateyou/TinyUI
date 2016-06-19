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
		/// <summary>
		/// ²Ã¼ôµÄ»­²¼
		/// </summary>
		class TinyClipCanvas : public TinyCanvas
		{
		public:
			TinyClipCanvas(HDC hDC, TinyVisual* spvis);
			virtual ~TinyClipCanvas();
			TinyRectangle	GetClipBox() const;
		private:
			HRGN			m_hRGN;
			TinyRectangle	m_clip;
		};


#define DECLARE_EVENT0(NAME,R)\
	Event<R(void)> EVENT_##NAME;
#define DECLARE_EVENT1(NAME,R,A1)\
	Event<R(A1)> EVENT_##NAME;
#define DECLARE_EVENT2(NAME,R,A1,A2)\
	Event<R(A1,A2)> EVENT_##NAME;
#define DECLARE_EVENT3(NAME,R,A1,A2,A3)\
	Event<R(A1,A2,A3)> EVENT_##NAME;
#define DECLARE_EVENT4(NAME,R,A1,A2,A3,A4)\
	Event<R(A1,A2,A3,A4)> EVENT_##NAME;

#define DECLARE_DELEGATE0(NAME,R)\
	Delegate<R(void)> DELEGATE_##NAME;
#define DECLARE_DELEGATE1(NAME,R,A1)\
	Delegate<R(A1)> DELEGATE_##NAME;
#define DECLARE_DELEGATE2(NAME,R,A1,A2)\
	Delegate<R(A1,A2)> DELEGATE_##NAME;
#define DECLARE_DELEGATE3(NAME,R,A1,A2,A3)\
	Delegate<R(A1,A2,A3)> DELEGATE_##NAME;
#define DECLARE_DELEGATE4(NAME,R,A1,A2,A3,A4)\
	Delegate<R(A1,A2,A3,A4)> DELEGATE_##NAME;

#define  BIND_EVENT(NAME,SOURCE,F)\
	DELEGATE_##NAME.BindDelegate(&F);\
	if(SOURCE)\
		{\
		SOURCE->EVENT_##NAME += &DELEGATE_##NAME;\
		}

#define  UNBIND_EVENT(NAME,SOURCE,F)\
	if(SOURCE)\
		{\
		SOURCE->EVENT_##NAME -= &DELEGATE_##NAME\
		}
	}
}



