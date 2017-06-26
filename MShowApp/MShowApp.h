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
		BOOL			Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName);
		MShowWindow*	GetShow();
		INT				Run();
	private:
		BOOL			Uninitialize();
	private:
		MShowWindow			m_mshow;
		MShowController		m_controller;
		TinyMessageLoop		m_msgLoop;
	};
}

