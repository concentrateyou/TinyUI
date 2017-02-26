#pragma once
#include "DX11.h"
#include "DX11Image2D.h"
#include "Common/TinyString.h"
#include <Richedit.h>
using namespace DXFramework;

namespace DXApp
{
	class WindowScene : public DX11Image2D
	{
		DECLARE_DYNAMIC(WindowScene)
	public:
		WindowScene();
		virtual ~WindowScene();
		BOOL Initialize(DX11& dx11, HWND hWND);
		HWND GetCaptureWindow() const;
	public:
		BOOL Process(DX11& dx11) OVERRIDE;
		BOOL Render(DX11& dx11) OVERRIDE;
		void Clear(DX11& dx11) OVERRIDE;
	private:
		HWND			m_hWND;
	};
}



