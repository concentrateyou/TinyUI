#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualHWND.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyRichTextBox.h"
#include "Render/TinyRectTracker.h"

#include "FLVFile.h"
#include "Media/TinySoundPlayer.h"

using namespace TinyUI;
using namespace Decode;

class FLVFrame : public TinyControl
{
public:
	FLVFrame();
	virtual ~FLVFrame();
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
private:
	void OnAudioDone(BYTE* bits, LONG size, FLV_PARAM& param);
	void OnVideoDone(BYTE* bits, LONG size, FLV_PARAM& param);
private:
	Decode::FLVFile m_flv;
	TinyScopedPtr<Delegate<void(BYTE*, LONG, FLV_PARAM&)>>	m_audioDone;
	TinyScopedPtr<Delegate<void(BYTE*, LONG, FLV_PARAM&)>>	m_videoDone;
	Media::TinySoundPlayer m_player;
};

