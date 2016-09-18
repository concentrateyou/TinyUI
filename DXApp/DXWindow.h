#pragma once
#include "Control/TinyControl.h"
#include "DXGraphics.h"
#include "MediaCapture.h"
#include "x264Encode.h"
#include "aacEncode.h"
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
	void	OnPublish();
	void	OnRender();
private:
	DXGraphics						m_graphics;
	MediaCapture					m_mediaCapture;
	DX11Image						m_videoImage;
	TinyScopedPtr<PublishTask>		m_publishTask;
	TinyScopedPtr<RenderTask>		m_renderTask;
	TinyScopedPtr<DX11CaptureTask>	m_captureTask;
	TinyScopedPtr<I420Converter>	m_converter;
	RTMPPublisher					m_publisher;
	x264Encode						m_x264;
	aacEncode						m_aac;
	UINT							m_startTime;
};

