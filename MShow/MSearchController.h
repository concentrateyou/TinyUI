#pragma once
#include "MShowCommon.h"
#include "MSearchWindow.h"
using namespace TinyUI;

namespace MShow
{
	class MSearchWindow;
	/// <summary>
	/// Ԥ��
	/// </summary>
	class MSearchController
	{
		DISALLOW_COPY_AND_ASSIGN(MSearchController)
	public:
		MSearchController(MSearchWindow& view);
		virtual ~MSearchController();
		BOOL	Initialize();
	private:
		void OnMinimumClick(EventArgs& args);
		void OnCloseClick(EventArgs& args);
		void OnSearchClick(EventArgs& args);
		void GetPrograms();
	private:
		MSearchWindow&				m_view;
		TinyScopedPtr<Delegate<void(EventArgs&)>>   m_onMinimumClick;
		TinyScopedPtr<Delegate<void(EventArgs&)>>   m_onCloseClick;
		TinyScopedPtr<Delegate<void(EventArgs&)>>   m_onSearchClick;
	};
}



