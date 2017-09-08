#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualNative.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualNative, TinyVisual);

		TinyVisualNative::TinyVisualNative(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree),
			m_pWND(NULL)
		{

		}

		HRESULT TinyVisualNative::OnCreate()
		{
			return TinyVisual::OnCreate();
		}

		HRESULT TinyVisualNative::OnDestory()
		{
			return TinyVisual::OnDestory();
		}

		HRESULT TinyVisualNative::OnInitialize()
		{
			return TinyVisual::OnInitialize();
		}

		TinyControl* TinyVisualNative::GetWindow()
		{
			return m_pWND;
		}

		void TinyVisualNative::SetVisible(BOOL visible)
		{
			if (m_pWND != NULL)
			{
				ShowWindow(m_pWND->Handle(), visible ? SW_SHOW : SW_HIDE);
			}
			TinyVisual::SetVisible(visible);
		}

		TinyVisualNative::~TinyVisualNative()
		{

		}
		TinyString TinyVisualNative::RetrieveTag() const
		{
			return TinyVisualTag::NATIVEWINDOW;
		}
		BOOL TinyVisualNative::SetProperty(const TinyString& name, const TinyString& value)
		{
			return TinyVisual::SetProperty(name, value);
		}

		void TinyVisualNative::SetPosition(const TinyPoint& pos)
		{
			TinyVisual::SetPosition(pos);
			if (m_pWND != NULL)
			{
				TinyRectangle rectangle = this->GetWindowRect();
				m_pWND->SetPosition(rectangle.Position());
			}
		}

		void TinyVisualNative::SetSize(const TinySize& size)
		{
			TinyVisual::SetSize(size);
			if (m_pWND != NULL)
			{
				TinyRectangle rectangle = this->GetWindowRect();
				m_pWND->SetSize(rectangle.Size());
			}
		}
		void TinyVisualNative::SetWindow(TinyControl* pWND)
		{
			m_pWND = pWND;
			if (m_pWND != NULL)
			{
				TinyRectangle rectangle = this->GetWindowRect();
				m_pWND->SetPosition(rectangle.Position());
				m_pWND->SetSize(rectangle.Size());
				ShowWindow(m_pWND->Handle(), IsVisible() ? SW_SHOW : SW_HIDE);
			}
		}
		BOOL TinyVisualNative::OnDraw(HDC hDC, const RECT& rcPaint)
		{

			return TRUE;
		}

		HRESULT	TinyVisualNative::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualNative::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT TinyVisualNative::OnMouseLeave()
		{
			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualNative::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
	}
}
