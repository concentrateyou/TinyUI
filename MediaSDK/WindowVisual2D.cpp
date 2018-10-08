#include "stdafx.h"
#include "WindowVisual2D.h"

namespace MediaSDK
{
	IMPLEMENT_DYNAMIC(WindowVisual2D, IVisual2D);

	WindowVisual2D::WindowVisual2D(DX11& dx11)
		:m_dx11(dx11),
		m_bCompatibility(FALSE),
		m_windowDC(NULL)
	{
		m_hBitmaps[0] = m_hBitmaps[1] = NULL;
	}

	WindowVisual2D::~WindowVisual2D()
	{
	}

	XMFLOAT2 WindowVisual2D::GetTranslate()
	{
		return m_visual2D.GetTranslate();
	}

	XMFLOAT2 WindowVisual2D::GetSize()
	{
		return m_visual2D.GetSize();
	}

	XMFLOAT2 WindowVisual2D::GetScale()
	{
		return m_visual2D.GetScale();
	}
	void WindowVisual2D::SetTranslate(const XMFLOAT2& pos)
	{
		m_visual2D.SetTranslate(pos);
	}

	void WindowVisual2D::SetScale(const XMFLOAT2& pos)
	{
		m_visual2D.SetScale(pos);
	}
	void WindowVisual2D::SetName(LPCSTR pzName)
	{
		m_szNAME = pzName;
	}
	void WindowVisual2D::SetCompatible(BOOL bCompatibility)
	{
		m_bCompatibility = bCompatibility;
	}
	void WindowVisual2D::Select(const TinyString& className, const TinyString& exeName)
	{
		ZeroMemory(&m_target, sizeof(m_target));
		StrCpy(m_target.className, className.STR());
		StrCpy(m_target.exeName, exeName.STR());
	}

	BOOL WindowVisual2D::Open()
	{
		Close();
		EnumWindows(WindowVisual2D::EnumWindow, reinterpret_cast<LPARAM>(&m_target));
		if (!IsWindow(m_target.hWND))
			return FALSE;
		TinyRectangle s;
		GetClientRect(m_target.hWND, &s);
		m_size = s.Size();
		if (m_bCompatibility)
		{
			BITMAPINFO bi = { 0 };
			BITMAPINFOHEADER *bih = &bi.bmiHeader;
			bih->biSize = sizeof(BITMAPINFOHEADER);
			bih->biBitCount = 32;
			bih->biWidth = m_size.cx;
			bih->biHeight = m_size.cy;
			bih->biPlanes = 1;
			m_windowDC = CreateCompatibleDC(NULL);
			if (!m_windowDC)
				goto _ERROR;
			BYTE* bits = NULL;
			m_hBitmaps[0] = CreateDIBSection(m_windowDC, &bi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
			if (!m_hBitmaps[0])
				goto _ERROR;
			m_hBitmaps[1] = (HBITMAP)SelectObject(m_windowDC, m_hBitmaps[0]);
		}
		if (!m_visual2D.Create(m_dx11, m_size.cx, m_size.cy))
			goto _ERROR;
		CURSORINFO ci;
		ZeroMemory(&ci, sizeof(ci));
		ci.cbSize = sizeof(ci);
		if (!GetCursorInfo(&ci))
			goto _ERROR;
		HICON hICON = CopyIcon(ci.hCursor);
		BOOL bRes = m_cursor2D.Create(m_dx11, hICON);
		DestroyIcon(hICON);
		hICON = NULL;
		return bRes;
	_ERROR:
		Close();
		return FALSE;
	}

	BOOL WindowVisual2D::Tick(INT64& timestamp)
	{
		if (!IsWindow(m_target.hWND) || IsIconic(m_target.hWND))
			return FALSE;
		TinyRectangle s;
		GetClientRect(m_target.hWND, &s);
		TinySize size = s.Size();
		if (size != m_size)
		{
			m_size = size;
			m_visual2D.Destory();
			if (!m_visual2D.Create(m_dx11, m_size.cx, m_size.cy, DXGI_FORMAT_B8G8R8A8_UNORM))
				return FALSE;
		}
		HDC hDC = GetDC(m_target.hWND);
		if (hDC != NULL)
		{
			if (m_bCompatibility)
			{
				BitBlt(m_windowDC, 0, 0, m_size.cx, m_size.cy, hDC, 0, 0, SRCCOPY);
			}
			else
			{
				HDC visualDC = NULL;
				if (m_visual2D.GetDC(TRUE, visualDC))
				{
					BitBlt(visualDC, 0, 0, m_size.cx, m_size.cy, hDC, 0, 0, SRCCOPY);
					m_visual2D.ReleaseDC();
				}
			}
			ReleaseDC(m_target.hWND, hDC);
			hDC = NULL;
		}
		CURSORINFO ci;
		ZeroMemory(&ci, sizeof(ci));
		ci.cbSize = sizeof(ci);
		if (!GetCursorInfo(&ci))
			return FALSE;
		size = { static_cast<LONG>(m_cursor2D.GetSize().x),static_cast<LONG>(m_cursor2D.GetSize().y) };
		HICON hICON = CopyIcon(ci.hCursor);
		ICONINFO ii;
		ZeroMemory(&ii, sizeof(ii));
		if (!GetIconInfo(hICON, &ii))
			return FALSE;
		BOOL bRes = m_cursor2D.UpdateCursor(m_dx11, hICON, size);
		DestroyIcon(hICON);
		hICON = NULL;
		m_cursor2D.SetFlipH(m_visual2D.IsFlipH());
		m_cursor2D.SetFlipV(m_visual2D.IsFlipV());
		m_cursor2D.SetRotate(m_visual2D.GetRotate());
		m_cursor2D.SetScale(m_visual2D.GetScale());
		POINT windowPos = { 0 };
		ClientToScreen(m_target.hWND, &windowPos);
		POINT pos = ci.ptScreenPos;
		pos.x -= windowPos.x + static_cast<LONG>(ii.xHotspot);
		pos.y -= windowPos.y + static_cast<LONG>(ii.yHotspot);
		XMFLOAT2 scale = m_visual2D.GetScale();
		XMFLOAT2 translate = m_visual2D.GetTranslate();
		m_cursor2D.SetTranslate(XMFLOAT2(translate.x + static_cast<FLOAT>(pos.x) * scale.x, translate.y + static_cast<FLOAT>(pos.y) * scale.y));
		return bRes;
	}

	BOOL WindowVisual2D::Draw(DX11Graphics2D& g)
	{
		FLOAT blendFactor[4] = { 0.0F, 0.0F, 0.0F, 0.0F };
		m_dx11.AllowBlend(FALSE, blendFactor);
		if (g.DrawImage(m_visual2D))
		{
			m_dx11.AllowBlend(TRUE, blendFactor);
			return g.DrawImage(m_cursor2D);
		}
		return FALSE;
	}

	void WindowVisual2D::Close()
	{
		if (m_windowDC != NULL)
		{
			SelectObject(m_windowDC, m_hBitmaps[1]);
			DeleteDC(m_windowDC);
			m_windowDC = NULL;
		}
		SAFE_DELETE_OBJECT(m_hBitmaps[0]);
		m_hBitmaps[0] = m_hBitmaps[1] = NULL;
		m_visual2D.Destory();
		m_cursor2D.Destory();
	}

	LPCSTR WindowVisual2D::GetName()
	{
		return m_szNAME.CSTR();
	}

	BOOL CALLBACK WindowVisual2D::EnumWindow(HWND hwnd, LPARAM lParam)
	{
		LPWNDINFO ws = reinterpret_cast<LPWNDINFO>(lParam);
		if (!IsWindowVisible(hwnd))
			return TRUE;
		TCHAR szClass[MAX_PATH];
		TCHAR szExecutable[MAX_PATH];
		if (GetClassName(hwnd, szClass, MAX_PATH))
		{
			if (strncasecmp(szClass, ws->className, strlen(ws->className)) == 0)
			{
				DWORD dwPID = 0;
				DWORD dwTID = GetWindowThreadProcessId(hwnd, &dwPID);
				TinyProcess process;
				if (process.Open(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID))
				{
					DWORD size = MAX_PATH;
					if (QueryFullProcessImageName(process, 0, szExecutable, &size))
					{
						CHAR* pzName = PathFindFileName(szExecutable);
						if (strncasecmp(pzName, ws->exeName, strlen(pzName)) == 0)
						{
							ws->hWND = hwnd;
							ws->dwPID = dwPID;
							ws->dwTID = dwTID;
							IsWow64Process(process, &ws->bX86);
							ws->hProcess = process.Duplicate();
							return FALSE;
						}
					}
				}
			}
		}
		return TRUE;
	}
}
