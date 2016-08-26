#pragma once
#include "DXWindow.h"
#include "DXFramework.h"

BOOL LoadSeDebugPrivilege();

namespace DXFramework
{
	class DXApplication
	{
	public:
		DXApplication();
		~DXApplication();
	public:
		BOOL Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName);
		DXWindow* GetDXWindow();
		INT Run();
	private:
		BOOL Uninitialize();
	private:
		TinyMessageLoop					m_msgLoop;
		TinyScopedPtr<DXWindow>			m_window;
	};
}

