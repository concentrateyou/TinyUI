#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualDocument.h"
#include "TinyVisualScrollBar.h"
#include "TinyVisualManage.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualComboBox;

		class TinyVisualComboBoxHWND : public TinyVisualWindowless
		{
			friend class TinyVisualComboBoxHWND;
			DECLARE_DYNAMIC(TinyVisualComboBoxHWND)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualComboBoxHWND)
		public:
			TinyVisualComboBoxHWND();
			virtual ~TinyVisualComboBoxHWND();
			LPCSTR				RetrieveClassName() OVERRIDE;
			LPCSTR				RetrieveTitle() OVERRIDE;
			DWORD				RetrieveStyle() OVERRIDE;
			DWORD				RetrieveExStyle() OVERRIDE;
		public:
			LRESULT				OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT				OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT				OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT				OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT				OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;

		};
	}
}



