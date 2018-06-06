#pragma once
#include "TinyVisual.h"
#include "TinyVisualWindowless.h"

namespace TinyFramework
{
	namespace Windowless
	{
#define DEFAULT_MENUITEM_WIDTH	160
#define DEFAULT_MENUITEM_HEIGHT	25
		/// <summary>
		/// 菜单项
		/// </summary>
		class TinyVisualMenuItem : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualMenuItem)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualMenuItem)
		protected:
			TinyVisualMenuItem();
			TinyVisualMenuItem(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualMenuItem();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
		/// <summary>
		/// 可视化上下文菜单
		/// </summary>
		class TinyVisualContextMenu : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualContextMenu)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualContextMenu)
		protected:
			TinyVisualContextMenu();
			TinyVisualContextMenu(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualContextMenu();
			TinyString	RetrieveTag() const OVERRIDE;
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		public:
			BOOL		Add(const TinyString& text, COLORREF color = RGB(0, 0, 0));
			BOOL		Remove(TinyVisualMenuItem* spvis);
			void		RemoveAll();
		private:
			TinyArray<TinyVisualMenuItem*>	m_items;
		};
	}
}



