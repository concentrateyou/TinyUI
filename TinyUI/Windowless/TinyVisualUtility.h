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
		/// 裁剪的DC在当前元素最小的矩形上绘制
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
		/// 消息过滤器
		/// </summary>
		class TINY_NO_VTABLE TinyVisualFilter
		{
		public:
			virtual BOOL OnFilter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) = 0;
		};
		/// <summary>
		/// 可视化消息筛选器集合
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
	}
}



