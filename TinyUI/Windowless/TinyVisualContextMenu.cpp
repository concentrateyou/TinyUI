#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualContextMenu.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualMenuItem, TinyVisual);
		TinyVisualMenuItem::TinyVisualMenuItem()
		{

		}
		TinyVisualMenuItem::TinyVisualMenuItem(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}

		TinyUI::TinyString TinyVisualMenuItem::RetrieveTag() const
		{
			return TinyVisualTag::MENUITEM;
		}
		//////////////////////////////////////////////////////////////////////////
		IMPLEMENT_DYNAMIC(TinyVisualContextMenu, TinyVisual);
		TinyVisualContextMenu::TinyVisualContextMenu()
		{

		}
		TinyVisualContextMenu::TinyVisualContextMenu(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}

		TinyUI::TinyString TinyVisualContextMenu::RetrieveTag() const
		{
			return TinyVisualTag::CONTEXTMENU;
		}
	}
}
