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
			friend class TinyVisualComboBoxItem;
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
			LRESULT					OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		public:
			TinySize				GetItemSize() const;
			void					SetItemSize(const TinySize& itemsize);
			TinyVisualComboBoxItem*	Add(const TinyString& name, const TinyString& text);
			void					Remove(const TinyString& name);
			void					RemoveAll();
			void					Popup(const TinyPoint& pos, INT cy);
			BOOL					IsPopup() const;
			void					Unpopup();
		public:
			Event<void(TinyVisual*, EventArgs&)>	EVENT_SELECTCHANGED;
		private:
			void					AdjustLayout(INT dx, INT dy);
			void					AdjustItems(INT cx);
			void					OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos);
			void					OnItemClick(TinyVisual*spvis, EventArgs& args);
		private:
			UINT					m_count;
			UINT					m_iPos;
			TinySize				m_itemsize;
			TinyVisualComboBox*		m_owner;
			TinyVisual*				m_current;
			TinyVisualVScrollBar*	m_scrollbar;
			TinyScopedPtr<Delegate<void(BOOL, INT, INT, INT)>>		m_onPosChange;
			TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>	m_onItemClick;
		};
	}
}



