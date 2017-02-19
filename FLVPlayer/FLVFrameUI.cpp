#include "stdafx.h"
#include "FLVFrameUI.h"

namespace FLVPlayer
{
	FLVFrameUI::FLVFrameUI()
	{
	}


	FLVFrameUI::~FLVFrameUI()
	{
	}

	LRESULT FLVFrameUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PostQuitMessage(0);
		return FALSE;
	}

	BOOL FLVFrameUI::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	DWORD FLVFrameUI::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
	}

	DWORD FLVFrameUI::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
	}

	LPCSTR FLVFrameUI::RetrieveClassName()
	{
		return TEXT("FLVFrameUI");
	}

	LPCSTR FLVFrameUI::RetrieveTitle()
	{
		return TEXT("FLVFrameUI");
	}

	HICON FLVFrameUI::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT FLVFrameUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		/*TinySoundPlayer player;
		TinyWaveFile waveFile;
		waveFile.Open("D:\\123.wav");
		player.Initialize(m_hWND, waveFile.GetFormat(), 192000 * 2);
		TinyEvent m_event[2];
		m_event[0].CreateEvent();
		m_event[1].CreateEvent();
		DSBPOSITIONNOTIFY vals[2];
		vals[0].dwOffset = 192000 * 1 - 1;
		vals[0].hEventNotify = m_event[0];
		vals[1].dwOffset = 192000 * 2 - 1;
		vals[1].hEventNotify = m_event[1];
		if (player.SetNotifys(2, vals))
		{
			player.Play();
		}
		HANDLE handles[2] = { m_event[0],m_event[1] };
		BYTE bits[192000];
		do 
		{
			HRESULT hRes = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
			if (hRes >= WAIT_OBJECT_0 && hRes <= (WAIT_OBJECT_0 + 1))
			{
				LONG size = 0;
				if (waveFile.Read(bits, 192000, &size))
				{
					player.Fill(bits, size);
				}
			}
		} while (1);
		waveFile.Close();
		player.Close();*/

		m_task.Reset(new FLVDecodeTask(m_hWND));
		m_task->Submit();
		return FALSE;
	}

	LRESULT FLVFrameUI::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_task->Close(INFINITE);
		return FALSE;
	}

	LRESULT FLVFrameUI::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT s = { 0 };
		HDC hDC = BeginPaint(m_hWND, &s);


		EndPaint(m_hWND, &s);
		return FALSE;
	}

	LRESULT FLVFrameUI::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}