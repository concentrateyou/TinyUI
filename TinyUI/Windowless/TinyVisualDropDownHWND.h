#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "TinyVisualHWND.h"
#include "TinyVisualDocument.h"


namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualDropDownHWND : public TinyVisualHWND
		{
			DECLARE_DYNAMIC(TinyVisualDropDownHWND)
		public:
			TinyVisualDropDownHWND();
			virtual ~TinyVisualDropDownHWND();
			DWORD RetrieveStyle() OVERRIDE;
			DWORD RetrieveExStyle() OVERRIDE;
			LPCSTR RetrieveClassName() OVERRIDE;
			LPCSTR RetrieveTitle() OVERRIDE;
			HICON RetrieveIcon() OVERRIDE;
		public:
			LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		};
	}
}



