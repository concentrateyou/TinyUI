#include "../stdafx.h"
#include "TinyVisualHWND.h"
#include "TinyVisualWindow.h"
#include "TinyVisualTree.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualTree::TinyVisualTree(TinyVisualHWND* pWindow)
			:m_spvisWindow(NULL),
			m_spvisCapture(NULL),
			m_spvisFocus(NULL),
			m_pWindow(pWindow)
		{
			ASSERT(m_pWindow);
		}
		TinyVisualTree::~TinyVisualTree()
		{

		}
		BOOL TinyVisualTree::Initialize()
		{
			m_spvisWindow = new TinyVisualWindow(this);
			if (!m_spvisWindow) return FALSE;
			m_parse.Reset(new TinyVisualParse());
			m_parse->LoadFile("D:\\resource.xml");
			return m_parse->BuildVisualTree(this);
		}
		void TinyVisualTree::Uninitialize()
		{
			SAFE_DELETE(m_spvisWindow);
		}
		TinyVisualHWND*	TinyVisualTree::GetVisualHWND()
		{
			return m_pWindow;
		}
		void TinyVisualTree::LinkVisual(TinyVisual* spvis, TinyVisual* spvisInsert, TinyVisual**pspvisFirst)
		{
			if (*pspvisFirst == spvis)
			{
				return;
			}
			if (spvisInsert == PVISUAL_TOP)
			{
				spvis->m_spvisNext = *pspvisFirst;
				*pspvisFirst = spvis;
			}
			else
			{
				if (spvisInsert == PVISUAL_BOTTOM)
				{
					if ((spvisInsert = *pspvisFirst) == NULL)
					{
						spvis->m_spvisNext = *pspvisFirst;
						*pspvisFirst = spvis;
					}
				}
			}
			ASSERT(spvis != spvisInsert);
			ASSERT(spvis != spvisInsert->m_spvisNext);
			ASSERT(spvis->m_spvisParent == spvisInsert->m_spvisParent);
			spvis->m_spvisNext = spvisInsert->m_spvisNext;
			spvisInsert->m_spvisNext = spvis;
		}
		void TinyVisualTree::UnlinkVisual(TinyVisual* spvisUnlink, TinyVisual** pspvisFirst)
		{
			TinyVisual* pv = NULL;
			pv = *pspvisFirst;
			if (pv == spvisUnlink)
			{
				*pspvisFirst = spvisUnlink->m_spvisNext;
				spvisUnlink->m_spvisNext = NULL;
				return;
			}
			while (pv != NULL)
			{
				if (pv->m_spvisNext == spvisUnlink)
				{
					pspvisFirst = &pv->m_spvisNext;
					*pspvisFirst = spvisUnlink->m_spvisNext;
					spvisUnlink->m_spvisNext = NULL;
					return;
				}
				pv = pv->m_spvisNext;
			}
		}
		TinyVisual* TinyVisualTree::GetVisual(TinyVisual* spvis, UINT cmd) const
		{
			if (spvis == m_spvisWindow)
			{
				switch (cmd)
				{
				case CMD_FIRST:
					break;
				default:
					return NULL;
					break;
				}
			}
			TinyVisual* pv = NULL;
			BOOL fRebase = FALSE;
			switch (cmd)
			{
			case CMD_NEXT:
				pv = spvis->m_spvisNext;
				fRebase = TRUE;
				break;
			case CMD_FIRST:
				if (spvis->m_spvisParent)
				{
					pv = spvis->m_spvisParent;
					pv = pv->m_spvisChild;
				}
				break;
			case CMD_LAST:
				pv = GetPrevVisual(spvis, NULL);
				break;
			case CMD_PREV:
				pv = GetPrevVisual(spvis, spvis);
				break;
			case CMD_OWNER:
				pv = spvis->m_spvisOwner;
				fRebase = TRUE;
				break;
			case CMD_CHILD:
				pv = spvis->m_spvisChild;
				fRebase = TRUE;
				break;
			default:
				return NULL;
			}
			return NULL;
		}
		TinyVisual* TinyVisualTree::GetPrevVisual(TinyVisual* spvisList, TinyVisual* spvisFind) const
		{
			TinyVisual* spvisFound = NULL;
			TinyVisual* spvisNext = NULL;
			if (spvisList == NULL)
				return NULL;
			if (spvisList->m_spvisParent == NULL)
				return NULL;
			spvisNext = spvisList->m_spvisParent;
			spvisNext = spvisNext->m_spvisChild;
			while (spvisNext != NULL)
			{
				if (spvisNext == spvisFind)
					break;
				spvisFound = spvisNext;
				spvisNext = spvisNext->m_spvisNext;
			}
			return (spvisNext == spvisFind) ? spvisFound : NULL;
		}
		TinyVisual* TinyVisualTree::GetParent(TinyVisual* spvis) const
		{
			if (!spvis) return m_spvisWindow;
			return spvis->m_spvisParent;
		}
		TinyVisual* TinyVisualTree::SetParent(TinyVisual* spvis, TinyVisual* spvisParent)
		{
			ASSERT(spvis && spvisParent);
			if (spvisParent == NULL)
			{
				spvisParent = m_spvisWindow;
			}
			if (spvis == m_spvisWindow)
			{
				return NULL;
			}
			TinyVisual* pv = NULL;
			for (pv = spvisParent; pv != NULL; pv = pv->m_spvisParent)
			{
				if (spvis == pv)
				{
					return NULL;
				}
			}
			spvis->SetVisible(FALSE);
			TinyVisual* spvisOldParent = spvis->m_spvisParent;
			INT x, y;
			x = spvis->m_windowRect.left - spvisOldParent->m_windowRect.left;
			y = spvis->m_windowRect.top - spvisOldParent->m_windowRect.top;
			UnlinkVisual(spvis, &spvisOldParent->m_spvisChild);
			spvis->m_spvisParent = spvisParent;
			LinkVisual(spvis, NULL, &spvisParent->m_spvisChild);
			SetVisualPos(spvis, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			spvis->SetVisible(TRUE);
			return spvisOldParent;
		}
		BOOL TinyVisualTree::IsChild(TinyVisual* spvisParent, TinyVisual* spvis) const
		{
			while (spvis != NULL)
			{
				spvis = spvis->m_spvisParent;
				if (spvisParent == spvis)
					return TRUE;
			}
			return FALSE;
		}
		BOOL TinyVisualTree::IsVisible(TinyVisual* spvis) const
		{
			if (spvis == NULL)
				return TRUE;
			for (;;)
			{
				if (!spvis->IsVisible())
					return FALSE;
				spvis = spvis->m_spvisParent;
			}
			return TRUE;
		}
		BOOL TinyVisualTree::SetVisualPos(TinyVisual* spvis, INT  x, INT  y, INT  cx, INT  cy, DWORD dwFlags)
		{
			TinyVisual* spvisParent = spvis->m_spvisParent;
			if (!spvisParent || !m_pWindow) return FALSE;
			if (cx < 0)
				cx = 0;
			else if (cx > SHRT_MAX)
				cx = SHRT_MAX;
			if (cy < 0)
				cy = 0;
			else if (cy > SHRT_MAX)
				cy = SHRT_MAX;
			INT mycx = cx;
			INT mycy = cy;
			if (dwFlags & SWP_NOSIZE)
			{
				mycx = TO_CX(&spvis->m_windowRect);
				mycy = TO_CY(&spvis->m_windowRect);
			}
			if (x > SHRT_MAX)
				x = SHRT_MAX;
			else if (x < SHRT_MIN)
				x = SHRT_MIN;
			if (y > SHRT_MAX)
				y = SHRT_MAX;
			else if (y < SHRT_MIN)
				y = SHRT_MIN;
			INT myx = x;
			INT myy = y;
			if (dwFlags & SWP_NOMOVE)
			{
				myx = spvis->m_windowRect.left - spvisParent->m_windowRect.left;
				myy = spvis->m_windowRect.top - spvisParent->m_windowRect.top;
			}
			spvis->m_windowRect.left = spvisParent->m_windowRect.left + myx;
			spvis->m_windowRect.top = spvisParent->m_windowRect.top + myy;
			spvis->m_windowRect.right = spvis->m_windowRect.left + mycx;
			spvis->m_windowRect.bottom = spvis->m_windowRect.top + mycx;
			return TRUE;
		}
		TinyVisual*	TinyVisualTree::GetVisualByPos1(TinyVisual* spvis, INT x, INT y)
		{
			while (spvis != NULL)
			{
				spvis = GetVisualByPos2(spvis, x, y);
				if (spvis != NULL)
				{
					return spvis;
				}
				spvis = spvis->m_spvisNext;
			}
			return NULL;
		}
		TinyVisual*	TinyVisualTree::GetVisualByPos2(TinyVisual* spvis, INT x, INT y)
		{
			if (!spvis || !spvis->IsVisible() || !spvis->IsEnable())
			{
				return NULL;
			}
			POINT pos = { x, y };
			if (!PtInRect((LPRECT)&spvis->m_windowRect, pos))
			{
				return NULL;
			}
			if (spvis->m_hrgnClip != NULL && !PtInRegion(spvis->m_hrgnClip, x, y))
			{
				return NULL;
			}
			if (PtInRect((LPRECT)&spvis->m_windowRect, pos))
			{
				spvis = GetVisualByPos1(spvis->m_spvisChild, x, y);
				if (spvis != NULL)
				{
					return spvis;
				}
			}
			return NULL;
		}
		TinyVisual*	TinyVisualTree::GetVisualByPos(INT x, INT y)
		{
			return GetVisualByPos2(m_spvisWindow, x, y);
		}
		TinyVisual* TinyVisualTree::GetCapture() const
		{
			return m_spvisCapture;
		}
		TinyVisual*	TinyVisualTree::SetCapture(TinyVisual* pNew)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pNew != pv)
			{
				m_spvisCapture = pNew;
			}
			return pv;
		}
		TinyVisual* TinyVisualTree::GetFocus() const
		{
			return m_spvisFocus;
		}
		TinyVisual*	TinyVisualTree::SetFocus(TinyVisual* pNew)
		{
			TinyVisual* pv = m_spvisFocus;
			if (pNew != pv)
			{
				m_spvisFocus = pNew;
			}
			return pv;
		}
		HRESULT	TinyVisualTree::OnMouseMove(POINT pos)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnMouseMove(pos);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					pv->OnMouseMove(pos);
				}
			}
			return FALSE;
		}
		HRESULT TinyVisualTree::OnLButtonDown(POINT pos)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnLButtonDown(pos);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					pv->OnLButtonDown(pos);
				}
			}
			return FALSE;
		}
		HRESULT TinyVisualTree::OnLButtonUp(POINT pos)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnLButtonUp(pos);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					pv->OnLButtonUp(pos);
				}
			}
			return FALSE;
		}
		HRESULT TinyVisualTree::OnRButtonDown(POINT pos)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnRButtonDown(pos);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					pv->OnRButtonDown(pos);
				}
			}
			return FALSE;
		}
		HRESULT TinyVisualTree::OnRButtonUp(POINT pos)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnRButtonUp(pos);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					pv->OnRButtonUp(pos);
				}
			}
			return FALSE;
		}
	}
}
