#pragma once
#include "MShowCommon.h"
#include "MSearchWindow.h"
using namespace TinyUI;

namespace MShow
{
	class MSearchWindow;
	/// <summary>
	/// ‘§¿¿
	/// </summary>
	class MSearchController
	{
		DISALLOW_COPY_AND_ASSIGN(MSearchController)
	public:
		MSearchController(MSearchWindow& view);
		virtual ~MSearchController();
		BOOL	Initialize();
		BOOL	Uninitialize();
		void	Refresh();
	private:
		void OnMinimumClick(TinyVisual*, EventArgs& args);
		void OnCloseClick(TinyVisual*, EventArgs& args);
		void OnSearchClick(TinyVisual*, EventArgs& args);
		void OnItemClick(TinyVisual*, MouseEventArgs& args);
		void GetPrograms();
		void OnMessagePump();
		void OnDestory(UINT, WPARAM, LPARAM, BOOL&);
		BOOL Query(const string& sourceID, const string& programID, const string& logID, INT& count);
	private:
		TinyLock					m_lock;
		MSearchWindow&				m_view;
		TinyTaskBase				m_task;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onDestory;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onMinimumClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onCloseClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onSearchClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, MouseEventArgs&)>>	m_onItemClick;
	};
}



