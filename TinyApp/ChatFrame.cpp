#include "stdafx.h"
#include "ChatFrame.h"

ChatFrame::ChatFrame()
{
}


ChatFrame::~ChatFrame()
{
}

LRESULT ChatFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}

BOOL ChatFrame::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD ChatFrame::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}

DWORD ChatFrame::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}

LPCSTR ChatFrame::RetrieveClassName()
{
	return TEXT("ChatFrame");
}

LPCSTR ChatFrame::RetrieveTitle()
{
	return TEXT("ChatFrame");
}

HICON ChatFrame::RetrieveIcon()
{
	return NULL;
}

LRESULT ChatFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;


	BOOL bRes = m_image.Load("D:\\1.gif");
	/*m_textbox.Create(m_hWND, 20, 20, 400, 300);
	m_speek.Create(m_hWND, 20, 330, 100, 23);
	m_speek.SetText("Speek");

	m_onSpeek.Reset(new Delegate<void(void*, INT)>(this, &ChatFrame::OnSpeek));
	m_speek.EVENT_Click += m_onSpeek;

	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL,__uuidof(ISpVoice), (void**)&m_spVoice);*/

	return FALSE;
}

LRESULT ChatFrame::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	//m_speek.EVENT_Click -= m_onSpeek;
	return FALSE;
}

LRESULT ChatFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PAINTSTRUCT s = { 0 };
	HDC hDC = BeginPaint(m_hWND, &s);
	m_canvas.InitializeDC(hDC);
	m_canvas.DrawImage(m_image, 10, 10, m_image.GetSize().cx, m_image.GetSize().cy);
	EndPaint(m_hWND, &s);
	return FALSE;
}

LRESULT ChatFrame::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

void ChatFrame::OnSpeek(void*, INT)
{
	//ASSERT(m_spVoice);

	/*TinyString str;
	INT s = ::GetWindowTextLength(m_textbox.Handle());
	str.Resize(s + 1);
	::GetWindowText(m_textbox.Handle(), str.STR(), s + 1);
	string str1 = str.STR();
	wstring wstr = StringToWString(str1);
	HRESULT hRes = m_spVoice->Speak(&wstr[0], SPF_DEFAULT, NULL);
	INT a = 0;*/
}

