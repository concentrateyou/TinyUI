#include "stdafx.h"
#include "MImageElement.h"

namespace MShow
{
	IMPLEMENT_DYNAMIC(MImageElement, DX11Image2D);

	MImageElement::MImageElement(MImageController& controller)
		:m_controller(controller)
	{
	}

	MImageElement::~MImageElement()
	{
	}
	MImageController& MImageElement::GetController()
	{
		return m_controller;
	}
	BOOL MImageElement::Allocate(DX11& dx11)
	{
		HANDLE handle = m_controller.GetHandle();
		if (!handle)
			return FALSE;
		return this->Load(dx11, handle);
	}

	BOOL MImageElement::Process(DX11& dx11)
	{
		return DX11Image2D::Process(dx11);
	}

	void MImageElement::Deallocate(DX11& dx11)
	{
		this->Destory();
	}
}