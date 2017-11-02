#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualFrame.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyRichTextBox.h"
#include "Control/TinyScrollBar.h"
#include "Network/TinyHTTPRequest.h"
#include "Network/TinyHTTPResponse.h"
#include "Windowless/TinyVisualWND.h"
#include "WavePlayer.h"
#include "AudioDSP.h"
#include <sapi.h>
#include <sphelper.h>
#include "Media/TinyXAudio.h"
#include "Network/TinyHTTPClient.h"
#include "DX2D.h"
#include "WICTexture.h"
#include "Control/TinyShadow.h"
#pragma comment(lib,"sapi.lib")

using namespace TinyUI;
using namespace TinyUI::Windowless;
using namespace TinyUI::Network;
using namespace DXFramework;

class ChatFrame : public TinyVisualWND
{
public:
	ChatFrame();
	virtual ~ChatFrame();
	//5个创建函数
	DWORD RetrieveStyle() OVERRIDE;
	DWORD RetrieveExStyle() OVERRIDE;
	LPCSTR RetrieveClassName() OVERRIDE;
	LPCSTR RetrieveTitle() OVERRIDE;
	HICON RetrieveIcon() OVERRIDE;
	//事件
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
private:
	void OnMessagePump();
private:
	DX9Graphics2D	m_graphics2D;
	DX9Image2D		m_image2D;
	TinyTaskBase	m_task;
};

