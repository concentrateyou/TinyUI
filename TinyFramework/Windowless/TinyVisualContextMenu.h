#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualMenuItem;

		class TinyVisualContextMenu : public TinyVisualWindowless
		{
			friend class TinyVisualMenuItem;
			DECLARE_DYNAMIC(TinyVisualContextMenu)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualContextMenu)
		public:
			TinyVisualContextMenu();
			virtual ~TinyVisualContextMenu();
			LPCSTR				RetrieveClassName() OVERRIDE;
			LPCSTR				RetrieveTitle() OVERRIDE;
			DWORD				RetrieveStyle() OVERRIDE;
			DWORD				RetrieveExStyle() OVERRIDE;
		public:
			LRESULT				OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT				OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT				OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT				OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT				OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT				OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		public:
			void				OnInitialize() OVERRIDE;
			void				OnUninitialize() OVERRIDE;
		public:
			TinyVisual*			GetOwner();
			TinyVisualMenuItem* Add();//·Ö¸îÏß
			TinyVisualMenuItem* Add(const TinyString& name, const TinyString& text, TinyImage* icon = NULL);
			void				Remove(const TinyString& name);
			void				Popup(TinyVisual* spvis, const TinyPoint& pos);
			BOOL				IsPopup() const;
			void				Unpopup();
		private:
			void				OnItemClick(TinyVisual*, EventArgs& args);
		private:
			UINT					m_offsetX;
			UINT					m_offsetY;
			TinyVisual*				m_hover;
			TinyVisual*				m_owner;
			TinyVisualContextMenu*	m_contextNext;
			TinyVisualContextMenu*	m_contextPrev;
			TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>> m_onItemClick;
		};
	}
}



