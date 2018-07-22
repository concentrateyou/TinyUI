#include "stdafx.h"
#include "WindowElement.h"

namespace Bytedance
{
	WindowElement::WindowElement(DX11& dx11)
		:m_dx11(dx11)
	{
	
	}
	WindowElement::~WindowElement()
	{

	}

	LPCSTR	WindowElement::GetName()
	{
		return TEXT("WindowElement");
	}

	BOOL WindowElement::Open()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL WindowElement::Process()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL WindowElement::Close()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	DX11ImageElement2D* WindowElement::GetVisual()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}
