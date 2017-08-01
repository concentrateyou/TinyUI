#pragma once
#include "Control/TinyMenu.h"
#include "MImageView.h"
#include "Render/TinyImage.h"
using namespace DXFramework;

namespace MShow
{
	class MShowController;
	class MPreviewController;
	class MImageElement;
	/// <summary>
	/// 图片控制器
	/// </summary>
	class MImageController
	{
		friend class MPreviewController;
		friend class MShowController;
		DISALLOW_COPY_AND_ASSIGN(MImageController)
	public:
		MImageController(MImageView& view);
		virtual ~MImageController();
		BOOL		Initialize();
		BOOL		Open(LPCSTR pzFile);
		BOOL		Close();
		HANDLE		GetHandle();//共享的纹理
		TinyString	GetFile() const;
	private:
		void	OnAdd();
		void	OnRemove();
		void	OnImage();
		void	OnMenuClick(void*, INT wID);
		void	OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		static VOID CALLBACK TimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
	private:
		UINT			m_index;
		HANDLE			m_handle;
		TinyString		m_szFile;
		TinyMenu		m_popup;
		MImageView&		m_view;
		DX11Graphics2D	m_graphics;
		DX11Image2D		m_image2D;
		TinyEvent		m_signal;
		MImageElement*	m_pImage;
	private:
		TinyScopedPtr<Delegate<void(void*, INT)>>					m_onMenuClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onLButtonDBClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onRButtonDown;
	};
}


