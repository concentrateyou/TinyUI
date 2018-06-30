#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualMenu : public TinyVisualWindowless
		{
			DECLARE_DYNAMIC(TinyVisualMenu)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualMenu)
		public:
			TinyVisualMenu();
			~TinyVisualMenu();
			LPCSTR RetrieveClassName() OVERRIDE;
			LPCSTR RetrieveTitle() OVERRIDE;
			DWORD RetrieveStyle() OVERRIDE;
			DWORD RetrieveExStyle() OVERRIDE;
			void OnInitialize() OVERRIDE;
			void OnUninitialize() OVERRIDE;
			LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		public:
			TinyVisual* Add(const TinyString& name, const TinyString& text, TinyImage* image = NULL);
			void Remove(const TinyString& name);
			void Popup(const TinyPoint& pos);
		private:
			void Draw();
			void OnItemClick(TinyVisual*, EventArgs& args);
		private:
			UINT		m_offsetX;
			UINT		m_offsetY;
			TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>> m_onItemClick;
		};
	}
}



