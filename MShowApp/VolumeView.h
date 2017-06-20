#pragma once
#include "MPreviewController.h"
#include "Control/TinyTrackBar.h"
using namespace DXFramework;

namespace MShow
{
	class MPreviewController;
	/// <summary>
	/// 音量控制
	/// </summary>
	class VolumeView : public TinyControl
	{
		DISALLOW_COPY_AND_ASSIGN(VolumeView)
	public:
		VolumeView();
		virtual ~VolumeView();
		//5个创建函数
		DWORD RetrieveStyle() OVERRIDE;
		DWORD RetrieveExStyle() OVERRIDE;
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
		HICON RetrieveIcon() OVERRIDE;
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		//事件
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		void OnPosChange(void*, INT pos);
	public:
		Event<void(DWORD)>	EVENT_VOLUME;
	private:
		BOOL			m_bFlag;
		HICON			m_hICONS[2];
		TinySize		m_size;
		TinyTrackBar	m_trackBar;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onPosChange;
	};
}


