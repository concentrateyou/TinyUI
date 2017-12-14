#pragma once
#include "TinyVisual.h"
#include "TinyVisualWND.h"

namespace TinyUI
{
	namespace Windowless
	{
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
			LRESULT OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		public:
			BOOL SetShadow(TinyImage* image);
			TinyRectangle GetShadowBox() const;
			void SetShadowBox(const TinyRectangle& box);
			BOOL DrawShadow();
		private:
			TinyImage*				m_image;
			TinyRectangle			m_box;
		};
	}
}



