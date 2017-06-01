#pragma once
#include "resource.h"
#include "MShowWindow.h"
using namespace TinyUI;

namespace MShow
{
	class MShowApp
	{
	public:
		MShowApp();
		~MShowApp();
	public:
		BOOL			Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName);
		MShowWindow*	GetWindow();
		INT				Run();
	private:
		BOOL			Uninitialize();
	private:
		TinyMessageLoop				m_msgLoop;
		TinyScopedPtr<MShowWindow>	m_mshowUI;//Ö÷´°¿Ú
	};
}

