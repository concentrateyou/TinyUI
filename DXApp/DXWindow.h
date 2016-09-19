#pragma once
#include "Control/TinyControl.h"
#include "DXGraphics.h"
#include "DX11CaptureTask.h"
#include "MediaCapture.h"
#include "RenderTask.h"
#include "EncodePublishTask.h"
#include "RTMPClient.h"
using namespace DXFramework;

class DXWindow : public TinyControl
{
	DECLARE_DYNAMIC(DXWindow)
public:
	DXWindow();
	virtual ~DXWindow();
	DWORD RetrieveStyle() OVERRIDE;
	DWORD RetrieveExStyle() OVERRIDE;
	LPCSTR RetrieveClassName() OVERRIDE;
	LPCSTR RetrieveTitle() OVERRIDE;
	HICON RetrieveIcon() OVERRIDE;
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
public:
	BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
private:
	BOOL	BuildEvents();
	void	OnRender();
private:
	DXGraphics							m_graphics;
	MediaCapture						m_mediaCapture;
	DX11Image							m_videoImage;
	TinyScopedPtr<RenderTask>			m_renderTask;
	TinyScopedPtr<EncodePublishTask>	m_encodeTask;
	TinyScopedPtr<DX11CaptureTask>		m_captureTask;
};

