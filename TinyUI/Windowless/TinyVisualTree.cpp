#include "../stdafx.h"
#include "TinyVisualHWND.h"
#include "TinyVisualWindow.h"
#include "TinyVisualTree.h"
#include "TinyVisualCaption.h"

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
			Uninitialize();
		}
		BOOL TinyVisualTree::Initialize()
		{
			m_fs.Reset(new TinyVisualFactory(this));
			m_spvisWindow = new TinyVisualWindow(NULL, this);
			m_spvisWindow->SetName("Window");
			m_spvisWindow->SetStyleImage(NORMAL, "D:\\image.png");

			TinyVisual* ps1 = m_fs->Create(0, 0, 300, 30, m_spvisWindow, TinyVisualTag::CAPTION);
			ps1->SetName("Caption-1");

			TinyVisual* ps5 = m_fs->Create(10, 0, 200, 30, ps1, TinyVisualTag::CAPTION);
			ps5->SetName("Caption-1-2");

			m_click.Reset(new Delegate<void(void)>(this, &TinyVisualTree::onClick));

			TinyVisualSysButton* ps2 = static_cast<TinyVisualSysButton*>(m_fs->Create(0, 0, 30, 30, ps5, TinyVisualTag::SYSBUTTON));
			ps2->EVENT_Click += m_click;
			ps2->SetName("Min-1");
			ps2->SetStyleImage(NORMAL, "D:\\Develop\\GitHub\\TinyUI\\Resource\\sysbutton\\btn_mini_normal.png");
			ps2->SetStyleImage(HIGHLIGHT, "D:\\Develop\\GitHub\\TinyUI\\Resource\\sysbutton\\btn_mini_highlight.png");
			ps2->SetStyleImage(DOWN, "D:\\Develop\\GitHub\\TinyUI\\Resource\\sysbutton\\btn_mini_down.png");


			TinyVisual* ps3 = m_fs->Create(30, 0, 30, 30, ps5, TinyVisualTag::SYSBUTTON);
			ps3->SetName("Max-1");
			ps3->SetStyleImage(NORMAL, "D:\\Develop\\GitHub\\TinyUI\\Resource\\sysbutton\\btn_max_normal.png");
			ps3->SetStyleImage(HIGHLIGHT, "D:\\Develop\\GitHub\\TinyUI\\Resource\\sysbutton\\btn_max_highlight.png");
			ps3->SetStyleImage(DOWN, "D:\\Develop\\GitHub\\TinyUI\\Resource\\sysbutton\\btn_max_down.png");

			TinyVisual* ps4 = m_fs->Create(60, 0, 30, 30, ps5, TinyVisualTag::SYSBUTTON);
			ps4->SetName("Close-1");
			ps4->SetStyleImage(NORMAL, "D:\\Develop\\GitHub\\TinyUI\\Resource\\sysbutton\\btn_close_normal.png");
			ps4->SetStyleImage(HIGHLIGHT, "D:\\Develop\\GitHub\\TinyUI\\Resource\\sysbutton\\btn_close_highlight.png");
			ps4->SetStyleImage(DOWN, "D:\\Develop\\GitHub\\TinyUI\\Resource\\sysbutton\\btn_close_down.png");

			TinyVisual* ps6 = m_fs->Create(0, 30, 300, 30, m_spvisWindow, TinyVisualTag::CAPTION);
			ps6->SetName("Caption-3");

			this->Dump();

			TinyVisual* pv = GetVisualByName("Close-1");

			return TRUE;
		}
		void TinyVisualTree::Uninitialize()
		{
			m_fs->Destory(m_spvisWindow);
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
			LINKTOP:
				spvis->m_spvisNext = *pspvisFirst;
				*pspvisFirst = spvis;
			}
			else
			{
				if (spvisInsert == PVISUAL_BOTTOM)
				{
					if ((spvisInsert = *pspvisFirst) == NULL)
					{
						goto LINKTOP;
					}
				}
				ASSERT(spvis != spvisInsert);
				ASSERT(spvis != spvisInsert->m_spvisNext);
				ASSERT(spvis->m_spvisParent == spvisInsert->m_spvisParent);
				spvis->m_spvisNext = spvisInsert->m_spvisNext;
				spvisInsert->m_spvisNext = spvis;
			}
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
				case GW_CHILD:
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
			return pv;
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
		TinyVisual* TinyVisualTree::SetParent(TinyVisual* spvis, TinyVisual* spvisNewParent)
		{
			ASSERT(spvis);
			if (spvisNewParent == NULL)
			{
				spvisNewParent = m_spvisWindow;
			}
			if (spvis == m_spvisWindow)
			{
				return NULL;
			}
			TinyVisual* pvT = NULL;
			for (pvT = spvisNewParent; pvT != NULL; pvT = pvT->m_spvisParent)
			{
				if (spvis == pvT)
				{
					return NULL;
				}
			}
			spvis->SetVisible(FALSE);
			TinyVisual* spvisOldParent = spvis->m_spvisParent;
			UnlinkVisual(spvis, &spvisOldParent->m_spvisChild);
			spvis->m_spvisParent = spvisNewParent;
			LinkVisual(spvis, NULL, &spvisNewParent->m_spvisChild);
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
		BOOL TinyVisualTree::IsEnable(TinyVisual* spvis) const
		{
			if (spvis == NULL)
				return TRUE;
			for (;;)
			{
				if (!spvis->IsEnable())
					return FALSE;
				spvis = spvis->m_spvisParent;
			}
			return TRUE;
		}
		TinyVisual*	TinyVisualTree::GetVisualByName(const TinyString& name)
		{
			return GetVisualByName1(m_spvisWindow, name);
		}
		TinyVisual*	TinyVisualTree::GetVisualByName1(TinyVisual* spvis, const TinyString& name)
		{
			while (spvis != NULL)
			{
				TinyVisual* hspvis = GetVisualByName2(spvis, name);
				if (hspvis != NULL)
				{
					return hspvis;
				}
				spvis = spvis->m_spvisNext;
			}
			return NULL;
		}
		TinyVisual*	TinyVisualTree::GetVisualByName2(TinyVisual* spvis, const TinyString& name)
		{
			if (!spvis)
				return NULL;
			if (spvis->GetName() == name)
				return spvis;
			return GetVisualByName1(spvis->m_spvisChild, name);
		}
		TinyVisual*	TinyVisualTree::GetVisualByPos(INT x, INT y)
		{
			return GetVisualByPos1(m_spvisWindow, x, y);
		}
		void TinyVisualTree::ConvertToVisualPos(TinyVisual* spvis, TinyPoint& pos)
		{
			ASSERT(m_pWindow && spvis);
			TinyVisual* pv = spvis;
			while (pv != NULL)
			{
				TinyPoint pvPos = pv->GetPosition();
				pos.Offset(-pvPos.x, -pvPos.y);
				pv = pv->m_spvisParent;
			}
		}
		TinyVisual*	TinyVisualTree::GetVisualByPos1(TinyVisual* spvis, INT x, INT y)
		{
			while (spvis != NULL)
			{
				TinyVisual* hspvis = GetVisualByPos2(spvis, x, y);
				if (hspvis != NULL)
				{
					return hspvis;
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
			TinyRectangle rectangle;
			rectangle.SetRect(GetWindowPos(spvis), spvis->GetSize());
			if (!PtInRect((LPRECT)&rectangle, pos))
			{
				return NULL;
			}
			if (spvis->m_hrgnClip != NULL && !PtInRegion(spvis->m_hrgnClip, x, y))
			{
				return NULL;
			}
			TinyVisual* hspvis = GetVisualByPos1(spvis->m_spvisChild, x, y);
			if (hspvis != NULL)
				return hspvis;
			return spvis;
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
		TinyPoint TinyVisualTree::GetWindowPos(const TinyVisual* spvis)
		{
			TinyPoint pos = spvis->GetPosition();
			TinyVisual* ps = spvis->m_spvisParent;
			while (ps != NULL)
			{
				pos.Offset(ps->GetPosition());
				ps = ps->m_spvisParent;
			}
			return pos;
		}
		TinyRectangle TinyVisualTree::GetWindowRect(const TinyVisual* spvis)
		{
			TinySize size = spvis->GetSize();
			TinyPoint pos = GetWindowPos(spvis);
			return TinyRectangle(pos, size);
		}
		TinyPoint TinyVisualTree::GetScreenPos(const TinyVisual* spvis)
		{
			TinyPoint pos = GetWindowPos(spvis);
			ClientToScreen(m_pWindow->Handle(), &pos);
			return pos;
		}
		void TinyVisualTree::Render(TinyVisual* spvis, HDC hDC)
		{
			while (spvis != NULL)
			{
				spvis->OnDraw(hDC);
				Render(spvis->m_spvisChild, hDC);
				spvis = spvis->m_spvisNext;
			}
		}
		HRESULT	TinyVisualTree::OnMouseMove(TinyPoint pos, DWORD dwFlags)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnMouseMove(pos, dwFlags);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					ConvertToVisualPos(pv, pos);
					pv->OnMouseMove(pos, dwFlags);
				}
			}
			return FALSE;
		}
		HRESULT TinyVisualTree::OnLButtonDown(TinyPoint pos, DWORD dwFlags)
		{
			this->Dump();
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnLButtonDown(pos, dwFlags);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					ConvertToVisualPos(pv, pos);
					pv->OnLButtonDown(pos, dwFlags);
				}
			}
			return FALSE;
		}
		HRESULT TinyVisualTree::OnLButtonUp(TinyPoint pos, DWORD dwFlags)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnLButtonUp(pos, dwFlags);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					ConvertToVisualPos(pv, pos);
					pv->OnLButtonUp(pos, dwFlags);
				}
			}
			return FALSE;
		}
		HRESULT TinyVisualTree::OnRButtonDown(TinyPoint pos, DWORD dwFlags)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnRButtonDown(pos, dwFlags);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					ConvertToVisualPos(pv, pos);
					pv->OnRButtonDown(pos, dwFlags);
				}
			}
			return FALSE;
		}
		HRESULT TinyVisualTree::OnRButtonUp(TinyPoint pos, DWORD dwFlags)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnRButtonUp(pos, dwFlags);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					ConvertToVisualPos(pv, pos);
					pv->OnRButtonUp(pos, dwFlags);
				}
			}
			return FALSE;
		}
		HRESULT TinyVisualTree::OnMButtonDown(TinyPoint pos, DWORD dwFlags)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnMButtonDown(pos, dwFlags);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					ConvertToVisualPos(pv, pos);
					pv->OnMButtonDown(pos, dwFlags);
				}
			}
			return FALSE;
		}
		HRESULT	TinyVisualTree::OnMButtonUp(TinyPoint pos, DWORD dwFlags)
		{
			TinyVisual* pv = m_spvisCapture;
			if (pv != NULL)
			{
				pv->OnMButtonUp(pos, dwFlags);
			}
			else
			{
				pv = GetVisualByPos(pos.x, pos.y);
				if (pv != NULL)
				{
					ConvertToVisualPos(pv, pos);
					pv->OnMButtonUp(pos, dwFlags);
				}
			}
			return FALSE;
		}
		void TinyVisualTree::Dump()
		{
			TRACE("Dump-----\n");
			TinyRectangle rect;
			m_pWindow->GetWindowRect(rect);
			TRACE("window:%d,%d,%d,%d\n", rect.left, rect.top, rect.right, rect.bottom);
			INT deep = 0;
			TinyVisual* spvis = this->GetParent(NULL);
			TinyVisual* ps = spvis->m_spvisChild;
			while (ps)
			{
				Dump(ps, deep);
				ps = ps->m_spvisNext;
				deep = 0;
			}
			TRACE("Dump-----\n");
		}
		void TinyVisualTree::Dump(TinyVisual* spvis, INT& deep)
		{
			++deep;
			if (!spvis) return;
			TinyString className = spvis->RetrieveTag();
			TinyString name = spvis->GetName();
			TinyString str;
			for (INT i = 0; i < deep; i++)
			{
				str += "-";
			}
			str += "Tag:%s,Name:%s,Rectangle(%d,%d,%d,%d)\n";
			TinyRectangle bounds = GetWindowRect(spvis);
			TRACE(str.STR(), className.STR(), name.STR(), bounds.left, bounds.top, bounds.right, bounds.bottom);
			TinyVisual* ps = spvis->m_spvisChild;
			while (ps)
			{
				Dump(ps, deep);
				ps = ps->m_spvisNext;
			}
		}

		void TinyVisualTree::onClick()
		{

		}
	}
}
