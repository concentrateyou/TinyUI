#pragma once
#include "TinyVisual.h"
#include "TinyVisualWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		class ShadowDC : public TinyHandleHDC
		{
			DISALLOW_COPY_AND_ASSIGN(ShadowDC)
		public:
			ShadowDC(HWND hWND);
			virtual ~ShadowDC();
			void BeginDraw(INT cx, INT cy);
			void BeginDraw(const TinySize& size);
			void EndDraw();
			HDC	 GetMemDC() const;
		protected:
			HWND		m_hWND;
			HDC			m_hMemDC;
			HBITMAP		m_hBitmap;
			HBITMAP		m_hOldBitmap;
			TinySize	m_size;
		};
		/// <summary>
		/// ±êÌâÀ¸ÈÝÆ÷¿Ø¼þ
		/// </summary>
		class TinyVisualShadow : public TinyVisualWND
		{
			DISALLOW_COPY_AND_ASSIGN(TinyVisualShadow)
		public:
			TinyVisualShadow();
			virtual ~TinyVisualShadow();
			LPCSTR RetrieveClassName() OVERRIDE;
			LPCSTR RetrieveTitle() OVERRIDE;
			HICON RetrieveIcon() OVERRIDE;
			DWORD RetrieveStyle() OVERRIDE;
			DWORD RetrieveExStyle() OVERRIDE;
		public:
			LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		public:
			BOOL SetShadow(TinyImage* image);
			TinyRectangle GetShadowBox() const;
			void SetShadowBox(const TinyRectangle& box);
			BOOL DrawShadow();
		private:
			TinyImage*				m_image;
			TinyRectangle			m_box;
			TinyScopedPtr<ShadowDC>	m_shadowDC;
		};
	}
}



