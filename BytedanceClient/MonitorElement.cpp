#include "stdafx.h"
#include "MonitorElement.h"

namespace Bytedance
{
	MonitorElement::MonitorElement(DX11& dx11)
		:m_dx11(dx11)
	{
	}
	MonitorElement::~MonitorElement()
	{

	}

	LPCSTR	MonitorElement::name()
	{
		return TEXT("MonitorElement");
	}
}
