#include "stdafx.h"
#include "MVideoElement.h"

namespace MShow
{
	IMPLEMENT_DYNAMIC(MVideoElement, DX11Image2D);

	MVideoElement::MVideoElement(MVideoController& controller)
		:m_controller(controller),
		m_bPush(FALSE)
	{
	}

	MVideoElement::~MVideoElement()
	{
	}
	MVideoController& MVideoElement::GetController()
	{
		return m_controller;
	}
	void MVideoElement::SetPush(BOOL bVal)
	{
		m_bPush = bVal;
	}
	BOOL MVideoElement::IsPush() const
	{
		return m_bPush;
	}
	BOOL MVideoElement::Allocate(DX11& dx11)
	{
		HANDLE handle = m_controller.GetHandle();
		if (!handle)
			return FALSE;
		if (this->Load(dx11, handle))
		{
			TinyRectangle s;
			::GetClientRect(dx11.GetHWND(), &s);
			//this->SetScale(s.Size());
		}
	}

	BOOL MVideoElement::Process(DX11& dx11)
	{
		return DX11Image2D::Process(dx11);
	}

	void MVideoElement::Deallocate(DX11& dx11)
	{
		this->Destory();
	}

}