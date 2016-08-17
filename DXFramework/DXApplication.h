#pragma once
#include "DXWindow.h"
#include "DXFramework.h"
#include "DXIdleHandler.h"

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
	public:
		void onDXWindowCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void onDXWindowDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	private:
		TinyMessageLoop	m_msgLoop;
		DXIdleHandler	m_idle;
		TinyScopedPtr<DXWindow>			m_window;
		TinyScopedPtr<D3D10Graphics>	m_graphics;
	private:
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onDXWindowCreate;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onDXWindowDestory;
	};
}

