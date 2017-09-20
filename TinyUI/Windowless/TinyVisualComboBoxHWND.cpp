#include "../stdafx.h"
#include "TinyVisualComboBoxHWND.h"
#include "TinyVisualComboBox.h"
#include "../Render/TinyTransform.h"
#include "../Render/TinyCanvas.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualComboBoxHWND, TinyVisualHWND);

		TinyVisualComboBoxHWND::TinyVisualComboBoxHWND(TinyVisualComboBox* pOwner)
			:m_pOwner(pOwner),
			m_pVScrollbar(NULL),
			m_spvisCurrent(NULL),
			m_iNewPos(0)
		{
			AllowTracking(FALSE);
		}
		TinyVisualComboBoxHWND::~TinyVisualComboBoxHWND()
		{

		}
		DWORD TinyVisualComboBoxHWND::RetrieveStyle()
		{
			return (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}
		DWORD TinyVisualComboBoxHWND::RetrieveExStyle()
		{
			return (WS_EX_LEFT | WS_EX_LTRREADING);
		}
		LPCSTR TinyVisualComboBoxHWND::RetrieveClassName()
		{
			return TEXT("TinyVisualComboBoxHWND");
		}
		LPCSTR TinyVisualComboBoxHWND::RetrieveTitle()
		{
			return TEXT("TinyVisualComboBoxHWND");
		}
		HICON TinyVisualComboBoxHWND::RetrieveIcon()
		{
			return NULL;
		}

		void TinyVisualComboBoxHWND::OnInitialize()
		{

		}

		void TinyVisualComboBoxHWND::OnUninitialize()
		{
			if (m_pVScrollbar != NULL)
			{
				m_pVScrollbar->EVENT_PosChange -= m_onPosChange;
				m_document->Destory(m_pVScrollbar);
				m_pVScrollbar = NULL;
			}
		}
		BOOL TinyVisualComboBoxHWND::IsPopup()
		{
			return IsWindowVisible(m_hWND);
		}
		void TinyVisualComboBoxHWND::SetSelected(TinyVisualOption* spvis, BOOL bFlag)
		{
			if (m_spvisCurrent != spvis)
			{
				if (m_spvisCurrent != NULL)
				{
					m_spvisCurrent->SetSelected(FALSE);
				}
				m_spvisCurrent = spvis;
				if (m_spvisCurrent != NULL)
				{
					m_spvisCurrent->SetSelected(TRUE);
				}
				m_pOwner->SetText(spvis->GetText());
				m_pOwner->EVENT_SELECTCHANGED(m_spvisCurrent);
			}
		}
		TinyVisualOption* TinyVisualComboBoxHWND::GetSelected()
		{
			return m_spvisCurrent;
		}
		BOOL TinyVisualComboBoxHWND::SetPosition(const TinyPoint& pos, const TinySize& size)
		{
			BOOL bRes = ::SetWindowPos(m_hWND, HWND_TOPMOST, pos.x, pos.y, size.cx, size.cy, SWP_NOACTIVATE | SWP_SHOWWINDOW);
			::UpdateWindow(m_hWND);
			::SetActiveWindow(m_hWND);
			TinyVisual* spvis = m_document->GetParent(NULL);
			if (m_pVScrollbar == NULL)
			{
				m_pVScrollbar = static_cast<TinyVisualVScrollBar*>(m_document->Create(size.cx - 12, 0, 12, size.cy, TinyVisualTag::VSCROLLBAR, spvis));
				m_onPosChange.Reset(new Delegate<void(BOOL, INT, INT, INT)>(this, &TinyVisualComboBoxHWND::OnPosChange));
				m_pVScrollbar->EVENT_PosChange += m_onPosChange;
			}
			if (m_pVScrollbar != NULL)
			{
				m_pVScrollbar->SetPosition(TinyPoint(size.cx - 12, 0));
				m_pVScrollbar->SetSize(TinySize(12, size.cy));
				INT cy = m_pOwner->GetOptions().GetSize() * DEFAULT_OPTION_HEIGHT;
				if (cy > size.cy)
				{
					m_pVScrollbar->SetVisible(TRUE);
					AdjustOption(size.cx - 12);
					m_pVScrollbar->SetScrollInfo(0, cy - size.cy, DEFAULT_OPTION_HEIGHT, m_iNewPos);
					m_document->SetFocus(m_pVScrollbar);
				}
				else
				{
					m_pVScrollbar->SetVisible(FALSE);
					AdjustOption(size.cx);
				}
			}
			m_document->Redraw();
			return bRes;
		}
		void TinyVisualComboBoxHWND::AdjustOption(INT cx)
		{
			TinyVisual* spvis = m_document->GetVisual(NULL, CMD_CHILD);
			spvis = m_document->GetVisual(spvis, CMD_LAST);
			while (spvis != NULL)
			{
				if (spvis->IsKindOf(RUNTIME_CLASS(TinyVisualOption)))
				{
					TinySize size = spvis->GetSize();
					size.cx = cx;
					spvis->SetSize(size);
				}
				spvis = m_document->GetVisual(spvis, CMD_PREV);
			}
		}
		LRESULT TinyVisualComboBoxHWND::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return TinyVisualHWND::OnCreate(uMsg, wParam, lParam, bHandled);
		}

		LRESULT TinyVisualComboBoxHWND::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = TRUE;
			return HTCLIENT;
		}

		LRESULT TinyVisualComboBoxHWND::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return TinyVisualHWND::OnDestory(uMsg, wParam, lParam, bHandled);
		}

		LRESULT TinyVisualComboBoxHWND::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				EVENT_ACTIVE(ActiveEventArgs(FALSE));
			}
			if (LOWORD(wParam) == WA_ACTIVE)
			{
				EVENT_ACTIVE(ActiveEventArgs(TRUE));
			}
			if (LOWORD(wParam) == WA_CLICKACTIVE)
			{
				EVENT_ACTIVE(ActiveEventArgs(TRUE));
			}
			return TinyVisualHWND::OnActivate(uMsg, wParam, lParam, bHandled);
		}

		void TinyVisualComboBoxHWND::OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos)
		{
			AdjustLayout(0, iOldPos - iNewPos);
			m_iNewPos = iNewPos;
			m_document->Redraw();
		}
		void TinyVisualComboBoxHWND::AdjustLayout(INT dx, INT dy)
		{
			TinyVisual* spvis = m_document->GetVisual(NULL, CMD_CHILD);
			spvis = m_document->GetVisual(spvis, CMD_LAST);
			while (spvis != NULL)
			{
				if (spvis->IsKindOf(RUNTIME_CLASS(TinyVisualOption)))
				{
					TinyRectangle s = spvis->GetRectangle();
					s.OffsetRect(dx, dy);
					spvis->SetPosition(s.Position());
					spvis->SetSize(s.Size());
				}
				spvis = m_document->GetVisual(spvis, CMD_PREV);
			}
		}
	}
}
