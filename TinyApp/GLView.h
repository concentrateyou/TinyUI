#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyRichTextBox.h"
#include "Control/TinyScrollBar.h"
#include "LAVPlayer.h"
#include "DX11Image2D.h"
#include "DX11Line2D.h"
#include "DX11Rectangle2D.h"
#include "DX11RenderView.h"
#include "DX11Graphics2D.h"
#include "DX11NV12Video.h"

#include "GL.h"
#include "GLRectangle2D.h"
#include "GLTexture2D.h"
#include "GLGraphics2D.h"

using namespace LAV;
using namespace DXFramework;
using namespace GLFramework;



class GLView : public TinyControl
{
public:
	GLView();
	virtual ~GLView();
	//5个创建函数
	DWORD RetrieveStyle() OVERRIDE;
	DWORD RetrieveExStyle() OVERRIDE;
	LPCSTR RetrieveClassName() OVERRIDE;
	LPCSTR RetrieveTitle() OVERRIDE;
	HICON RetrieveIcon() OVERRIDE;
	BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
	//事件
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
private:
	void OnTimer();
	static HHOOK g_hhk;
	static LRESULT CALLBACK MouseFilterHook(INT code, WPARAM wParam, LPARAM lParam);

private:
	/*DX11Graphics2D		m_graphics;
	DX11NV12Video		m_image2D;*/

	GLGraphics2D		m_graphics;
	//GLImage2D			m_image;
	GLRectangle2D		m_rectangle2D;
	GLLine2D			m_line2D;
	//TinyTimer			m_timer;


	//DX11Image2D		m_image2D;
	//DX11Line2D		m_line2D;
	//DX11Rectangle2D	m_rectangle2D;
	///*DX9Graphics2D	m_graphics;
	//DX9Image2D		m_image2D[2];*/
	//LAVPlayer m_player;
};