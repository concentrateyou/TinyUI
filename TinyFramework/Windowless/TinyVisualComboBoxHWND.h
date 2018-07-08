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
		class TinyVisualComboBoxItem;
		class TinyVisualComboBox;

		class TinyVisualComboBoxHWND : public TinyVisualWindowless
		{
			friend class TinyVisualComboBox;
			friend class TinyVisualComboBoxHWND;
			DECLARE_DYNAMIC(TinyVisualComboBoxHWND)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualComboBoxHWND)
		public:
			TinyVisualComboBoxHWND();
			virtual ~TinyVisualComboBoxHWND();
			LPCSTR					RetrieveClassName() OVERRIDE;
			LPCSTR					RetrieveTitle() OVERRIDE;
			DWORD					RetrieveStyle() OVERRIDE;
			DWORD					RetrieveExStyle() OVERRIDE;
		public:
			void					OnInitialize() OVERRIDE;
			void					OnUninitialize() OVERRIDE;
		public:
			LRESULT					OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT					OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		public:
			TinyVisualComboBoxItem*	Add(const TinyString& name, const TinyString& text);
			void					Remove(const TinyString& name);
			void					RemoveAll();
			void					Popup(const TinyPoint& pos);
			BOOL					IsPopup() const;
			void					Unpopup();
		private:
			UINT					m_offsetX;
			UINT					m_offsetY;
			TinyVisualComboBox*		m_owner;
		};
	}
}



