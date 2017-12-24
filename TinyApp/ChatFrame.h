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
#include "Windowless/TinyVisualWND.h"
#include "WavePlayer.h"
#include "AudioDSP.h"
#include <sapi.h>
#include <sphelper.h>
#include "Media/TinyXAudio.h"
#include "Network/TinyHTTPClient.h"
#include "DX2D.h"
#include "WICTexture.h"
#include "DX9Graphics2D.h"
#include "DX9Image2D.h"
#include "DX9RenderView.h"
#include "Media/TinyMFIntelQSVDecode.h"
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
	void OnTimerPump();
	void OnData(BYTE* bits, LONG size, LPVOID ps);
private:
	DX9Graphics2D			m_graphics2D;
	DX9Image2D				m_image2D;
	TinyTask			m_task;
	TinyTaskTimer			m_timer;
	TinyMFIntelQSVDecode	m_qsv;
	

};

