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
		QWORD				GetQPCTimeNS();
		QWORD				GetQPCTimeMS();
		QWORD				GetQPCTime100NS();
		BOOL				SleepNS(QWORD qwNSTime);
	public:
		static MShowApp&	GetInstance() throw();
	private:
		MShowWindow			m_window;
		MShowController		m_controller;
		TinyMessageLoop		m_msgLoop;
	};
}

