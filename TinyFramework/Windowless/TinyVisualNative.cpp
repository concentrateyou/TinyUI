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
		{

		}
		TinyVisualNative::TinyVisualNative(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
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
			if (m_hWND != NULL)
			{
				::SetWindowPos(m_hWND, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
			TinyVisual::SetPosition(pos);
		}

		void TinyVisualNative::SetSize(const TinySize& size)
		{
			if (m_hWND != NULL)
			{
				::SetWindowPos(m_hWND, NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER);
			}
			TinyVisual::SetSize(size);
		}

		void TinyVisualNative::SetVisible(BOOL visible)
		{
			if (m_hWND)
			{
				::ShowWindow(m_hWND, visible ? SW_SHOW : SW_HIDE);
			}
			TinyVisual::SetVisible(visible);
		}

		void TinyVisualNative::SetEnable(BOOL enable)
		{
			if (m_hWND != NULL)
			{
				EnableWindow(m_hWND, enable);
			}
			TinyVisual::SetEnable(enable);
		}

	}
}
