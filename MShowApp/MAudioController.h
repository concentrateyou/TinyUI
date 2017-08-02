#pragma once
#include "Control/TinyMenu.h"
#include "MAudioView.h"
using namespace DXFramework;

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
	private:
		void	OnAdd();
		void	OnRemove();
		void	OnMenuClick(void*, INT wID);
		void	OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	private:
		TinyString		m_szFile;
		TinyMenu		m_popup;
		MAudioView&		m_view;
	private:
		TinyScopedPtr<Delegate<void(void*, INT)>>					m_onMenuClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onLButtonDBClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onRButtonDown;
	};
}


