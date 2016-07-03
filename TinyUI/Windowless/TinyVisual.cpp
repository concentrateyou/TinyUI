#include "../stdafx.h"
#include "TinyVisual.h"
#include "TinyVisualTree.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisual::TinyVisual(TinyVisual* spvisParent, TinyVisualTree* vtree)
			:m_spvisParent(spvisParent),
			m_spvisNext(NULL),
			m_spvisChild(NULL),
			m_spvisOwner(NULL),
			m_hrgnClip(NULL),
			m_vtree(vtree),
			m_visible(TRUE),
			m_enable(TRUE)
		{

		}
		TinyVisual::~TinyVisual()
		{

		}
		void TinyVisual::SetText(LPCSTR pzText)
		{
			m_strText = pzText;
		}
		TinyString	TinyVisual::GetText() const
		{
			return m_strText;
		}
		void TinyVisual::SetName(LPCSTR pzName)
		{
			m_strName = pzName;
		}
		TinyString	TinyVisual::GetName() const
		{
			return m_strName;
		}
		void TinyVisual::SetToolTip(LPCSTR pzTitle)
		{
			m_strToolTip = pzTitle;
		}
		TinyString	TinyVisual::GetToolTip() const
		{
			return m_strToolTip;
		}
		void TinyVisual::SetMaximumSize(const TinySize& size)
		{
			if (m_maximumSize != size)
			{
				m_maximumSize = size;
			}
		}
		void TinyVisual::SetMinimumSize(const TinySize& size)
		{
			if (m_minimumSize != size)
			{
				m_minimumSize = size;
			}
		}
		TinySize TinyVisual::GetMaximumSize() const
		{
			return m_maximumSize;
		}
		TinySize TinyVisual::GetMinimumSize() const
		{
			return m_minimumSize;
		}
		BOOL TinyVisual::IsVisible() const
		{
			return m_visible;
		}
		BOOL TinyVisual::IsEnable() const
		{
			return m_enable;
		}
		void TinyVisual::SetVisible(BOOL visible)
		{
			if (m_visible != visible)
			{
				m_visible = visible;
				for (TinyVisual* pv = this->m_spvisChild; pv != NULL; pv = pv->m_spvisNext)
				{
					if (!pv) continue;;
					pv->SetVisible(visible);
				}
			}
		}
		void TinyVisual::SetEnable(BOOL enable)
		{
			if (m_enable != enable)
			{
				m_enable = enable;
				for (TinyVisual* pv = this->m_spvisChild; pv != NULL; pv = pv->m_spvisNext)
				{
					if (!pv) continue;;
					pv->SetEnable(enable);
				}
			}
		}
		TinyPoint TinyVisual::GetPosition() const
		{
			return *((TinyPoint*)&m_windowRect);
		}
		void TinyVisual::SetPosition(const TinyPoint& newPos)
		{
			TinyPoint& pos = *((TinyPoint*)&m_windowRect);
			if (pos != newPos)
			{
				pos.x = newPos.x;
				pos.y = newPos.y;
				OnPosChange(pos, newPos);
			}
		}
		TinySize TinyVisual::GetSize() const
		{
			return m_windowRect.Size();
		}
		void TinyVisual::SetSize(const TinySize& newsize)
		{
			TinySize size = *((TinySize*)&m_windowRect + 1);
			if (size != newsize)
			{
				m_windowRect.SetSize(newsize);
				OnSizeChange(size, newsize);
			}
		}
		TinyRectangle TinyVisual::GetRectangle() const
		{
			return m_windowRect;
		}
		TinyVisualTree*	TinyVisual::GetVisualTree()
		{
			return m_vtree;
		}
		void TinyVisual::SetClip(HRGN hrgnClip)
		{
			this->m_hrgnClip = hrgnClip;
		}
		HRGN TinyVisual::GetClip() const
		{
			return m_hrgnClip;
		}
		BOOL TinyVisual::SetStyleImage(StyleImage type, LPCSTR pzFile)
		{
			return m_images[(INT)type].Load(pzFile);
		}
		BOOL TinyVisual::SetStyleImage(StyleImage type, BYTE*	ps, DWORD dwSize)
		{
			return m_images[(INT)type].Load(ps, dwSize);
		}
		void TinyVisual::OnPosChange(const TinyPoint&oldPos, const TinyPoint&newPos)
		{

		}
		void TinyVisual::OnSizeChange(const TinySize&oldSize, const TinySize&newSize)
		{

		}
		void TinyVisual::Resize()
		{

		}
		BOOL	TinyVisual::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return FALSE;
		}
		HRESULT	TinyVisual::OnMouseMove(TinyPoint pos, DWORD dwFlags)
		{
			MouseButtons button = NONE;
			if (dwFlags & MK_LBUTTON) button = MouseButtons::LBUTTON;
			if (dwFlags & MK_RBUTTON) button = MouseButtons::RBUTTON;
			if (dwFlags & MK_MBUTTON) button = MouseButtons::MBUTTON;
			EVENT_MouseMove(MouseEventArgs(button, 0, pos.x, pos.y, 0));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMouseEnter()
		{
			return FALSE;
		}
		HRESULT	TinyVisual::OnMouseLeave()
		{
			return FALSE;
		}
		HRESULT	TinyVisual::OnLButtonDown(TinyPoint pos, DWORD dwFlags)
		{
			EVENT_MouseDown(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnLButtonUp(TinyPoint pos, DWORD dwFlags)
		{
			EVENT_MouseUp(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			EVENT_MouseClick(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnLButtonDBClick(TinyPoint pos, DWORD dwFlags)
		{
			EVENT_MouseDoubleClick(MouseEventArgs(MouseButtons::LBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonDown(TinyPoint pos, DWORD dwFlags)
		{
			EVENT_MouseDown(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonUp(TinyPoint pos, DWORD dwFlags)
		{
			EVENT_MouseUp(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			EVENT_MouseClick(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnRButtonDBClick(TinyPoint pos, DWORD dwFlags)
		{
			EVENT_MouseDoubleClick(MouseEventArgs(MouseButtons::RBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonDown(TinyPoint pos, DWORD dwFlags)
		{
			EVENT_MouseDown(MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonUp(TinyPoint pos, DWORD dwFlags)
		{
			EVENT_MouseUp(MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 1));
			return FALSE;
		}
		HRESULT	TinyVisual::OnMButtonDBClick(TinyPoint pos, DWORD dwFlags)
		{
			EVENT_MouseDoubleClick(MouseEventArgs(MouseButtons::MBUTTON, 0, pos.x, pos.y, 2));
			return FALSE;
		}
		HRESULT	TinyVisual::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_KeyDown(KeyEventArgs(dwChar));
			return FALSE;
		}
		HRESULT	TinyVisual::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			EVENT_KeyUp(KeyEventArgs(dwChar));
			return FALSE;
		}
	}
}
