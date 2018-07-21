#pragma once
#include "Common/TinyModule.h"
#include "IElement.h"
#include "DX11.h"
#include "DX11CaptureRunner.h"
#include "DX11Image2D.h"
#include "DX11YUY2Video.h"
#include "DShowCommon.h"
#include "VideoCapture.h"
using namespace TinyFramework;
using namespace TinyFramework::IO;
using namespace DXFramework;
using namespace DShow;

namespace Bytedance
{
	/// <summary>
	/// ´°¿ÚÔªËØ
	/// </summary>
	class WindowElement : public IElement
	{
		DISALLOW_COPY_AND_ASSIGN(WindowElement)
	public:
		WindowElement(DX11& dx11);
		virtual ~WindowElement();
		
		BOOL				Open() OVERRIDE;
		BOOL				Process() OVERRIDE;
		BOOL				Close() OVERRIDE;
		LPCSTR				name() OVERRIDE;
		DX11ImageElement2D*	visual() OVERRIDE;
	private:
		DX11&	m_dx11;
	};
}


