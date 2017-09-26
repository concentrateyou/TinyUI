#pragma once
#include "MShowCommon.h"
#include "MClientWindow.h"
#include "MPreviewView.h"
#include "MPreviewController.h"
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
		BOOL	Uninitialize();
		void	UpdateItem(SEARCH_ITEM& item);
	private:
		void InitializeUI();
		void OnMinimumClick(TinyVisual*, EventArgs& args);
		void OnCloseClick(TinyVisual*, EventArgs& args);
	private:
		SEARCH_ITEM							m_item;
		MClientWindow&						m_view;
		TinyScopedPtr<MPreviewController>	m_preview;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>   m_onMinimumClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>   m_onCloseClick;
	};

}

