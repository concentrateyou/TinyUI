#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "TinyVisualHWND.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualPopupHWND : public TinyVisualHWND
		{
			DECLARE_DYNAMIC(TinyVisualPopupHWND)
		public:
			TinyVisualPopupHWND();
			virtual ~TinyVisualPopupHWND();
			DWORD RetrieveStyle() OVERRIDE;
			DWORD RetrieveExStyle() OVERRIDE;
			LPCSTR RetrieveClassName() OVERRIDE;
			LPCSTR RetrieveTitle() OVERRIDE;
			HICON RetrieveIcon() OVERRIDE;
			void OnInitialize() OVERRIDE;
			void OnUninitialize() OVERRIDE;
		public:
			BOOL SetPosition(const TinyPoint& pos, const TinySize& size);
		public:
			LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		};
	}
}



