#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "TinyVisualHWND.h"
#include "TinyVisualDocument.h"
#include "TinyVisualScrollBar.h"
#include "TinyVisualManage.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualComboBox;
		class TinyVisualOption;
		/// <summary>
		/// ComboBoxÏÂÀ­¿ò
		/// </summary>
		class TinyVisualComboBoxHWND : public TinyVisualHWND
		{
			DECLARE_DYNAMIC(TinyVisualComboBoxHWND)
			friend class TinyVisualOption;
		public:
			TinyVisualComboBoxHWND(TinyVisualComboBox* pOwner);
			virtual ~TinyVisualComboBoxHWND();
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
			TinyVisualVScrollBar* GetScrollBar();
			Event<void(ActiveEventArgs&)>	EVENT_ACTIVE;
		public:
			LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		private:
			void AdjustOption(INT cx);
			void AdjustLayout(INT dx, INT dy);
			void OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos);
		private:
			INT						m_iNewPos;
			TinyVisualOption*		m_pCurrent;
			TinyVisualComboBox*		m_pOwner;
			TinyVisualVScrollBar*	m_pVScrollbar;
			TinyScopedPtr<Delegate<void(BOOL, INT, INT, INT)>> m_onPosChange;
		};
	}
}



