#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualNative.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualNative, TinyVisual);

		TinyVisualNative::TinyVisualNative()
			:m_view(NULL)
		{

		}
		TinyVisualNative::TinyVisualNative(TinyVisual* spvisParent, TinyVisualDocument* document)
			: TinyVisual(spvisParent, document),
			m_view(NULL)
		{

		}
		TinyVisualNative::~TinyVisualNative()
		{

		}

		TinyString TinyVisualNative::RetrieveTag() const
		{
			return TinyVisualTag::NATIVE;
		}
		BOOL TinyVisualNative::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return TinyVisual::OnDraw(hDC, rcPaint);
		}

		void TinyVisualNative::SetPosition(const TinyPoint& pos)
		{
			if (m_view != NULL)
			{
				::SetWindowPos(m_view->Handle(), NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
			TinyVisual::SetPosition(pos);
		}

		void TinyVisualNative::SetSize(const TinySize& size)
		{
			if (m_view != NULL)
			{
				::SetWindowPos(m_view->Handle(), NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER);
			}
			TinyVisual::SetSize(size);
		}

		void TinyVisualNative::SetVisible(BOOL visible)
		{
			if (m_view != NULL)
			{
				::ShowWindow(m_view->Handle(), visible ? SW_SHOW : SW_HIDE);
			}
			TinyVisual::SetVisible(visible);
		}

		void TinyVisualNative::SetEnable(BOOL enable)
		{
			if (m_view != NULL)
			{
				EnableWindow(m_view->Handle(), enable);
			}
			TinyVisual::SetEnable(enable);
		}
		void TinyVisualNative::SetView(TinyControl* view)
		{
			m_view = view;
		}
	}
}
