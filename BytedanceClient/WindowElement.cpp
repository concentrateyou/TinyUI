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

	LPCSTR	WindowElement::name()
	{
		return TEXT("WindowElement");
	}
}
