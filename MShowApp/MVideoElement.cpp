#include "stdafx.h"
#include "MVideoElement.h"

namespace MShow
{
	IMPLEMENT_DYNAMIC(MVideoElement, DX11Image2D);

	MVideoElement::MVideoElement(MVideoController& controller)
		:m_controller(controller)
	{
	}

	MVideoElement::~MVideoElement()
	{
	}
	MVideoController& MVideoElement::GetController()
	{
		return m_controller;
	}
	BOOL MVideoElement::Allocate(DX11& dx11)
	{
		HANDLE handle = m_controller.GetHandle();
		if (!handle)
			return FALSE;
		return this->Load(dx11, handle);
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