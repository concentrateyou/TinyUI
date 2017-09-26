#pragma once
#include "resource.h"
#include "MShowWindow.h"
#include "MShowController.h"
#include "MSearchWindow.h"
#include "MSearchController.h"
#include "MClientWindow.h"
#include "MClientController.h"
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
		MSearchWindow&		GetSearchView();
		MSearchController&	GetSearchController();
		MClientWindow&		GetClientView();
		MClientController&	GetClientController();
		INT					Run();
		QWORD				GetQPCTimeNS();
		QWORD				GetQPCTimeMS();
		QWORD				GetQPCTime100NS();
		BOOL				SleepNS(QWORD qwNSTime);
		void				SetCurrentAudioTS(LONGLONG ts);
		LONGLONG			GetCurrentAudioTS();
		string				GetDefaultPath();
		MAppConfig&			AppConfig();
	public:
		static MShowApp&	GetInstance() throw();
	private:
		LONGLONG			m_audioTS;
		string				m_szPath;
		TinyLock			m_lock;
		MShowWindow			m_window;
		MShowController		m_controller;
		MSearchWindow		m_searchView;
		MSearchController	m_searchCTRL;
		MClientWindow		m_clientView;
		MClientController	m_clientCRTL;
		TinyMessageLoop		m_msgLoop;
		MAppConfig			m_appConfig;
	};
}

