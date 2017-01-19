#pragma once
#include "DX11.h"
#include "DX11Image.h"
#include "Common/TinyString.h"
#include <Richedit.h>
using namespace DXFramework;

namespace DXApp
{
	class WindowScene : public DX11Image
	{
	public:
		WindowScene();
		virtual ~WindowScene();
		BOOL Initialize(DX11& dx11, HWND hWND);
	public:
		LPCSTR GetClassName() OVERRIDE;
		BOOL BeginScene() OVERRIDE;
		void EndScene() OVERRIDE;
		BOOL Render(DX11& dx11) OVERRIDE;
	private:
		HWND m_hWND;
	};
}



