#pragma once
#include "Control/TinyMenu.h"
#include "MAudioView.h"
#include "MP3DecodeTask.h"
using namespace DXFramework;
using namespace Decode;
using namespace TinyUI::Media;

namespace MShow
{
	/// <summary>
	/// ÒôÆµ¿ØÖÆÆ÷
	/// </summary>
	class MAudioController
	{
		friend class MPreviewController;
		friend class MShowController;
		DISALLOW_COPY_AND_ASSIGN(MAudioController)
	public:
		MAudioController(MAudioView& view);
		virtual ~MAudioController();
		BOOL		Initialize();
		BOOL		Open(LPCSTR pzFile);
		BOOL		Close();
		TinyString	GetFile() const;
	public:
		Event<void(BYTE*, LONG)>	EVENT_AUDIO;
	private:
		void	OnAdd();
		void	OnRemove();
		void	OnPlay();
		void	OnAudio(BYTE*, LONG);
		void	OnMenuClick(void*, INT wID);
		void	OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	private:
		TinyString		m_szFile;
		TinyMenu		m_popup;
		MAudioView&		m_view;
		MP3DecodeTask	m_task;
	private:
		TinyScopedPtr<Delegate<void(void*, INT)>>					m_onMenuClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onLButtonDBClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onRButtonDown;
	};
}


