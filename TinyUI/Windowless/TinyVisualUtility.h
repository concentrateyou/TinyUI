#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"

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
		class TinyVisual;
		/// <summary>
		/// �ü���DC�ڵ�ǰԪ����С�ľ����ϻ���
		/// </summary>
		class TinyClipCanvas : public TinyCanvas
		{
		public:
			TinyClipCanvas(HDC hDC, TinyVisual* spvis, const RECT& rcPaint);
			virtual ~TinyClipCanvas();
		private:
			HRGN	m_hRGN;
			HRGN	m_hOldRGN;
		};
		/// <summary>
		/// ��Ϣ������
		/// </summary>
		class NO_VTABLE TinyVisualFilter
		{
		public:
			virtual BOOL OnFilter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) = 0;
		};
		/// <summary>
		/// ���ӻ���Ϣɸѡ������
		/// </summary>
		class TinyVisualFilters
		{
		public:
			TinyVisualFilters();
			~TinyVisualFilters();
			BOOL Add(TinyVisualFilter* element);
			BOOL Remove(TinyVisualFilter* element);
			BOOL RemoveAt(INT index);
			void RemoveAll();
			INT  GetSize() const;
			const TinyVisualFilter* operator[] (INT index) const;
			TinyVisualFilter* operator[] (INT index);
			INT Lookup(TinyVisualFilter* element) const;
		private:
			TinyArray<TinyVisualFilter*> m_array;
		};
		/// <summary>
		/// ����
		/// </summary>
		template<class T>
		class TinyAttribute
		{
		public:
			TinyAttribute()
				:m_value(NULL)
			{

			}
			~TinyAttribute()
			{
				if (m_value != NULL)
					m_value->~T();
				SAFE_FREE(m_value);
			}
			void SetAttribute(const T& myT)
			{
				m_value = (T*)malloc(sizeof(T));
				if (m_value != NULL)
				{
#pragma push_macro("new")
#undef new
					::new(m_value) TinyPlaceNew<T>(myT);
#pragma pop_macro("new")
				}
			}
			void SetAttribute(T&& myT)
			{
				m_value = (T*)malloc(sizeof(T));
				if (m_value != NULL)
				{
#pragma push_macro("new")
#undef new
					::new(m_value) TinyPlaceNew<T>(std::move(myT));
#pragma pop_macro("new")
				}

			}
			void SetNull()
			{
				if (m_value != NULL)
					m_value->~T();
				SAFE_FREE(m_value);
			}
			BOOL IsNull() const
			{
				return m_value == NULL;
			}
			operator T*() const
			{
				return m_value;
			}
		private:
			T*	m_value;
		};
	}
}



