#pragma once
#include "MShowCommon.h"
#include "MSearchWindow.h"
using namespace TinyUI;

namespace MShow
{
	typedef struct tagSEARCH_ITEM
	{
		CHAR	Name[MAX_PATH];
		LONG	BeginTime;
		LONG	EndTime;
	}SEARCH_ITEM;

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
	private:
		void OnMinimumClick(TinyVisual*, EventArgs& args);
		void OnCloseClick(TinyVisual*, EventArgs& args);
		void OnSearchClick(TinyVisual*, EventArgs& args);
		void OnItemClick(TinyVisual*, MouseEventArgs& args);
		void GetPrograms();
		void OnMessagePump();
	private:
		MSearchWindow&				m_view;
		TinyTaskBase				m_task;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onMinimumClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onCloseClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onSearchClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, MouseEventArgs&)>>	m_onItemClick;
	};
}



