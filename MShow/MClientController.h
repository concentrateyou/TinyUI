#pragma once
#include "MShowCommon.h"
#include "MClientWindow.h"
using namespace TinyUI;

namespace MShow
{
	class MClientWindow;

	class MClientController
	{
	public:
		DISALLOW_COPY_AND_ASSIGN(MClientController)
	public:
		MClientController(MClientWindow& view);
		virtual ~MClientController();
		BOOL	Initialize();
	private:
		void OnMinimumClick(TinyVisual*, EventArgs& args);
		void OnCloseClick(TinyVisual*, EventArgs& args);
	private:
		MClientWindow&				m_view;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>   m_onMinimumClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>   m_onCloseClick;
	};

}

