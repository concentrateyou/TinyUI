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
			:TinyVisual(spvisParent, vtree)
		{

		}

		HRESULT TinyVisualNative::OnCreate()
		{
			m_control.Create(m_document->GetVisualHWND()->Handle(), 0, 0, 1, 1, FALSE);
			return TinyVisual::OnCreate();
		}

		HRESULT TinyVisualNative::OnDestory()
		{
			m_control.DestroyWindow();
			return TinyVisual::OnDestory();
		}

		HRESULT TinyVisualNative::OnInitialize()
		{
			TinyRectangle rectangle = this->GetWindowRect();
			m_control.SetPosition(rectangle.Position());
			m_control.SetSize(rectangle.Size());
			return TinyVisual::OnInitialize();
		}

		TinyControl& TinyVisualNative::GetNative()
		{
			return m_control;
		}

		void TinyVisualNative::SetVisible(BOOL visible)
		{
			ShowWindow(m_control.Handle(), visible ? SW_SHOW : SW_HIDE);
			TinyVisual::SetVisible(visible);
		}

		TinyVisualNative::~TinyVisualNative()
		{

		}
		TinyString TinyVisualNative::RetrieveTag() const
		{
			return TinyVisualTag::NATIVE;
		}
		BOOL TinyVisualNative::SetProperty(const TinyString& name, const TinyString& value)
		{
			return TinyVisual::SetProperty(name, value);
		}

		void TinyVisualNative::SetPosition(const TinyPoint& pos)
		{
			TinyVisual::SetPosition(pos);

		}

		void TinyVisualNative::SetSize(const TinySize& size)
		{
			TinyVisual::SetSize(size);

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
