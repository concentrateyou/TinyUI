#include "../stdafx.h"
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisual::TinyVisual()
			:m_spvisNext(NULL),
			m_spvisParent(NULL),
			m_spvisChild(NULL),
			m_spvisOwner(NULL),
			m_hrgnClip(NULL),
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
				TinyVisual* pv = NULL;
				for (pv = pv->m_spvisChild; pv != NULL; pv = pv->m_spvisNext)
				{
					pv->SetVisible(visible);
				}
			}
		}
		void TinyVisual::SetEnable(BOOL enable)
		{
			if (m_enable != enable)
			{
				m_enable = enable;
				TinyVisual* pv = NULL;
				for (pv = pv->m_spvisChild; pv != NULL; pv = pv->m_spvisNext)
				{
					pv->SetEnable(enable);
				}
			}
		}
		//LPCSTR	TinyVisual::RetrieveTag()
		//{
		//	return TEXT("");
		//}
		//HRESULT	TinyVisual::OnDraw(TinyDC& dc, TinyRectangle& drawRect)
		//{
		//	return FALSE;
		//}
		//HRESULT TinyVisual::OnMouseMove(POINT pos)
		//{
		//	return FALSE;
		//}
		//HRESULT TinyVisual::OnLButtonDown(POINT pos)
		//{
		//	return FALSE;
		//}
		//HRESULT TinyVisual::OnLButtonUp(POINT pos)
		//{
		//	return FALSE;
		//}
		//HRESULT TinyVisual::OnRButtonDown(POINT pos)
		//{
		//	return FALSE;
		//}
		//HRESULT TinyVisual::OnRButtonUp(POINT pos)
		//{
		//	return FALSE;
		//}
	}
}
