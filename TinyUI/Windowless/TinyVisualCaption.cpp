#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualCaption.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualCaption::TinyVisualCaption(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualCaption::~TinyVisualCaption()
		{

		}
		TinyString TinyVisualCaption::RetrieveTag() const
		{
			return TinyVisualTag::SYSCAPTION;
		}
		BOOL TinyVisualCaption::IsLayout() const
		{
			return TRUE;
		}
		void TinyVisualCaption::OnSizeChange(const TinySize& oldsize, const TinySize& newsize)
		{
			TinyVisual::OnSizeChange(oldsize, newsize);
			/*TinyVisual* spvis = m_spvisChild;
			while (spvis != NULL && spvis->IsVisible())
			{
				if (spvis->IsLayout())
				{

				}
				spvis = m_document->GetVisual(spvis, CMD_NEXT);
			}*/
		}

		BOOL TinyVisualCaption::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return TRUE;
		}

		HRESULT	TinyVisualCaption::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			HWND hWND = m_document->GetVisualHWND()->Handle();
			::SendMessage(hWND, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
			::SendMessage(hWND, WM_LBUTTONUP, dwFlags, MAKELPARAM(pos.x, pos.y));
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualCaption::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT TinyVisualCaption::OnMouseLeave()
		{

			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualCaption::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
	}
}
