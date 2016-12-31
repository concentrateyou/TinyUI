#pragma once
#include "DXFrameUI.h"
#include "DXFramework.h"

class DXApplication
{
public:
	DXApplication();
	~DXApplication();
public:
	BOOL		Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName);
	DXFrameUI*	GetFrameUI();
	INT			Run();
private:
	BOOL		Uninitialize();
private:
	TinyMessageLoop				m_msgLoop;
	TinyScopedPtr<DXFrameUI>	m_frameUI;
};