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

	LPCSTR	MonitorElement::GetName()
	{
		return TEXT("MonitorElement");
	}

	BOOL MonitorElement::Open()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL MonitorElement::Process()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL MonitorElement::Close()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	DX11ImageElement2D* MonitorElement::GetVisual()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}
