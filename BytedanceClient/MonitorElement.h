#pragma once
#include "Common/TinyModule.h"
#include "IVisual.h"
#include "DX11.h"
#include "DX11CaptureRunner.h"
#include "DX11Image2D.h"
#include "DX11Duplicator.h"
using namespace TinyFramework;
using namespace TinyFramework::IO;
using namespace DXFramework;

namespace Bytedance
{
	/// <summary>
	/// ÏÔÊ¾Æ÷²¶»ñ
	/// </summary>
	class MonitorElement : public IVisual
	{
		DISALLOW_COPY_AND_ASSIGN(MonitorElement)
	public:
		MonitorElement(DX11& dx11);
		virtual ~MonitorElement();
		UINT			GetMonitors() const;
		BOOL			Select(UINT index);
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Process() OVERRIDE;
		BOOL			Close() OVERRIDE;
		LPCSTR			GetName() OVERRIDE;
		DX11Element*	GetVisual() OVERRIDE;
	private:
		DX11&			m_dx11;
		DX11Image2D		m_monitor2D;
		DX11Duplicator	m_duplicator;
	};
}


