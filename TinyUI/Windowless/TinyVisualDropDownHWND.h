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
		/// <summary>
		/// ������
		/// </summary>
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
			void OnInitialize() OVERRIDE;
			void OnUninitialize() OVERRIDE;
			BOOL IsPopup();
		public:
			BOOL SetPosition(const TinyPoint& pos, const TinySize& size);
			Event<void(ActiveEventArgs&)>	EVENT_ACTIVE;
		public:
			LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		};
	}
}



