#pragma once
#include "TinyVisual.h"
#include "TinyVisualWindowless.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// 菜单项
		/// </summary>
		class TinyVisualMenuItem : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualMenuItem)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualMenuItem)
		protected:
			TinyVisualMenuItem();
			TinyVisualMenuItem(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			TinyString RetrieveTag() const OVERRIDE;

		};
		/// <summary>
		/// 可视化上下文菜单
		/// </summary>
		class TinyVisualContextMenu : public TinyVisualWindowless
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualContextMenu)
		public:
			TinyVisualContextMenu();
			virtual ~TinyVisualContextMenu();
			LPCSTR RetrieveClassName() OVERRIDE;
			LPCSTR RetrieveTitle() OVERRIDE;
			HICON RetrieveIcon() OVERRIDE;
			DWORD RetrieveStyle() OVERRIDE;
			DWORD RetrieveExStyle() OVERRIDE;
			void OnInitialize() OVERRIDE;
			void OnUninitialize() OVERRIDE;
		public:
			BOOL Popup(const TinyPoint& pos);
			BOOL IsPopup();
			BOOL Unpopup();
		public:
			LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
			LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		private:
			TinyArray<TinyVisualMenuItem*>	m_items;
		};
	}
}



