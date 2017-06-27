#pragma once
#include "MShowWindow.h"
#include "MVideoController.h"
#include "MShowController.h"
using namespace TinyUI;

namespace MShow
{
	class MShowApp
	{
	public:
		MShowApp();
		~MShowApp();
	public:
		BOOL				Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName);
		BOOL				Uninitialize();
		MShowWindow&		GetView();
		MShowController&	GetController();
		INT					Run();
	public:
		static MShowApp& Instance() throw();
	private:
		MShowWindow			m_window;
		MShowController		m_controller;
		TinyMessageLoop		m_msgLoop;
	};
}

