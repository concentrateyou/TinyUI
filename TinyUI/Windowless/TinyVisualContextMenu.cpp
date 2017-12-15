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
		IMPLEMENT_DYNAMIC(TinyVisualContextMenu, TinyVisualWindowless);

		TinyVisualContextMenu::TinyVisualContextMenu()
		{

		}
		TinyVisualContextMenu::~TinyVisualContextMenu()
		{

		}
		LPCSTR TinyVisualContextMenu::RetrieveClassName()
		{
			return TEXT("TinyVisualContextMenu");
		}

		LPCSTR TinyVisualContextMenu::RetrieveTitle()
		{
			return TEXT("TinyVisualContextMenu");
		}

		HICON TinyVisualContextMenu::RetrieveIcon()
		{
			return NULL;
		}

		DWORD TinyVisualContextMenu::RetrieveStyle()
		{
			return (WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX);
		}

		DWORD TinyVisualContextMenu::RetrieveExStyle()
		{
			return (WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
		}

		void TinyVisualContextMenu::OnInitialize()
		{

		}

		void TinyVisualContextMenu::OnUninitialize()
		{

		}

		LRESULT TinyVisualContextMenu::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return TinyVisualWindowless::OnCreate(uMsg, wParam, lParam, bHandled);
		}

		LRESULT TinyVisualContextMenu::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return TinyVisualWindowless::OnDestory(uMsg, wParam, lParam, bHandled);
		}

		BOOL TinyVisualContextMenu::Popup(const TinyPoint& pos)
		{
			this->SetPosition(pos);
			this->SetSize({ 400,400 });
			DrawContext();
			return TRUE;
		}

		BOOL TinyVisualContextMenu::IsPopup()
		{
			return TRUE;
		}

		BOOL TinyVisualContextMenu::Unpopup()
		{
			return TRUE;
		}

		BOOL TinyVisualContextMenu::DrawContext()
		{
			if (!m_hWND || !m_visualDC)
				return FALSE;
			if (!m_document)
				return FALSE;
			TinyRectangle windowRect;
			::GetWindowRect(m_hWND, &windowRect);
			TinyRectangle rectangle = windowRect;
			rectangle.OffsetRect(-rectangle.left, -rectangle.top);
			m_document->Draw(m_visualDC, rectangle);
			TinyPoint pos;
			TinyPoint dstPos = windowRect.Position();
			TinySize  dstSize = windowRect.Size();
			BLENDFUNCTION bs = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			BOOL bRes = ::UpdateLayeredWindow(m_hWND, m_visualDC->Handle(), &dstPos, &dstSize, m_visualDC->GetMemDC(), &pos, 0, &bs, 2);
			return bRes;
		}
	}
}
