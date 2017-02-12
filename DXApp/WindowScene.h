#pragma once
#include "DX11.h"
#include "DX11Image.h"
#include "Common/TinyString.h"
#include <Richedit.h>
using namespace DXFramework;

namespace DXApp
{
	/*typedef INT(WINAPI* DwmpDxGetWindowSharedSurface)(HWND, LUID, DWORD, DWORD, DXGI_FORMAT*, HANDLE*, LPVOID*);
	typedef INT(WINAPI* DwmpDxUpdateWindowSharedSurface)(HWND, INT, INT, INT, HMONITOR, void*);*/

	class WindowScene : public DX11Image
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
		HWND	m_hWND;
		//HANDLE	m_handle;
		/*DwmpDxGetWindowSharedSurface m_pDwmpDxGetWindowSharedSurface;
		DwmpDxUpdateWindowSharedSurface m_pDwmpDxUpdateWindowSharedSurface;*/
	};
}



