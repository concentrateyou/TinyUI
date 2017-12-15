#pragma once
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "MFLVPlayer.h"
#include "Control/TinyMenu.h"
using namespace DXFramework;
using namespace TinyUI;

namespace MShow
{
	class MPreviewView;
	/// <summary>
	/// ‘§¿¿
	/// </summary>
	class MPreviewController
	{
		friend class MShowController;
		DISALLOW_COPY_AND_ASSIGN(MPreviewController)
	public:
		MPreviewController(MPreviewView& view);
		virtual ~MPreviewController();
		BOOL			IsPlaying() const;
		BOOL			Open(LPCSTR pzURL);
		BOOL			Open(LPCSTR pzURL, Callback<void(BYTE*, LONG)>&& audioCB);
		BOOL			Close();
		MPreviewView&	GetView();
		MFLVPlayer*		GetPlayer();
	private:		
		MPreviewView&				m_view;
		TinyScopedPtr<MFLVPlayer>	m_player;
	};
}



