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
		class TinyVisualOption;
		/// <summary>
		/// ComboBox������
		/// </summary>
		class TinyVisualComboBoxHWND : public TinyVisualWindowless
		{
			DECLARE_DYNCREATE(TinyVisualComboBoxHWND)
			friend class TinyVisualOption;
			friend class TinyVisualComboBox;
		public:
			TinyVisualComboBoxHWND(TinyVisualComboBox* pOwner);
			virtual ~TinyVisualComboBoxHWND();
			DWORD	RetrieveStyle() OVERRIDE;
			DWORD	RetrieveExStyle() OVERRIDE;
			LPCSTR	RetrieveClassName() OVERRIDE;
			LPCSTR	RetrieveTitle() OVERRIDE;
			HICON	RetrieveIcon() OVERRIDE;
			void	OnInitialize() OVERRIDE;
			void	OnUninitialize() OVERRIDE;
			BOOL	IsPopup();
		public:
			BOOL	SetPosition(const TinyPoint& pos, const TinySize& size);
			void	SetSelected(TinyVisualOption* spvis);
			TinyVisualOption* GetSelected();
		public:
			Event<void(ActiveEventArgs&)>	EVENT_ACTIVE;
		public:
			LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		private:
			void	AdjustOption(INT cx);
			void	AdjustLayout(INT dx, INT dy);
			void	OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos);
		private:
			INT						m_iNewPos;
			TinyVisualOption*		m_spvisCurrent;
			TinyVisualComboBox*		m_pOwner;
			TinyVisualVScrollBar*	m_pVScrollbar;
			TinyScopedPtr<Delegate<void(BOOL, INT, INT, INT)>> m_onPosChange;
		};
	}
}



