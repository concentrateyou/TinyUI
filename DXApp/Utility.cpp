#include "stdafx.h"
#include "Utility.h"

BOOL WINAPI GetModuleList(HANDLE hProcess, TinyArray<TinyString> &moduleList)
{
	HMODULE hModules[1024];
	DWORD dwCount = 0;
	if (EnumProcessModulesEx(hProcess, hModules, sizeof(hModules), &dwCount, LIST_MODULES_ALL))
	{
		for (UINT i = 0; i < (dwCount / sizeof(HMODULE)); i++)
		{
			TinyString szFileName(MAX_PATH);
			if (GetModuleFileNameEx(hProcess, hModules[i], szFileName.STR(), MAX_PATH))
			{
				TinyString str = szFileName.Substring(szFileName.LastIndexOf("\\") + 1, szFileName.GetSize());
				moduleList.Add(str);
			}
		}
		return TRUE;
	}
	return FALSE;
}

typedef struct tagWND
{
	HWND   hWND;
	POINT  pos;
	DWORD  area;
	BOOL   bShowHidden;
}WND;

static BOOL CALLBACK FindChildProc(HWND hwnd, LPARAM lParam)
{
	WND* wnd = (WND*)lParam;
	POINT pos;
	pos.x = LOWORD(lParam);
	pos.y = HIWORD(lParam);
	RECT  rect;
	GetWindowRect(hwnd, &rect);
	if (PtInRect(&rect, pos))
	{
		DWORD area = TO_CX(rect) * TO_CY(rect);
		if (area < wnd->area && (IsWindowVisible(hwnd) || wnd->bShowHidden))
		{
			wnd->area = area;
			wnd->hWND = hwnd;
		}
	}
	return TRUE;
}

HWND FindChild(HWND hWND, WND& wnd)
{
	HWND  hParent = NULL;
	DWORD dwStyle = 0;
	hParent = GetParent(hWND);
	dwStyle = GetWindowLong(hWND, GWL_STYLE);
	if (hParent == NULL || (dwStyle & WS_POPUP))
	{
		hParent = hWND;
	}
	EnumChildWindows(hParent, FindChildProc, (LPARAM)&wnd);
	return wnd.hWND;
}
HWND WINAPI WindowFromPointEx(POINT pos, BOOL bShowHidden)
{
	HWND hWND;
	hWND = WindowFromPoint(pos);
	if (hWND == NULL)
		return NULL;
	WND wnd = { hWND, pos,-1,bShowHidden };
	hWND = FindChild(hWND, wnd);
	if (!bShowHidden)
	{
		while (hWND && !IsWindowVisible(hWND))
			hWND = GetParent(hWND);
	}
	return hWND;
}
BOOL WINAPI InvertWindow(HWND hWND)
{
	RECT rect;
	RECT rect2;
	RECT rect3;
	HDC hDC = NULL;
	INT x1, y1;
	INT border = 3;
	GetWindowRect(hWND, &rect);
	GetClientRect(hWND, &rect3);
	ClientToScreen(hWND, (POINT *)&rect3.left);
	ClientToScreen(hWND, (POINT *)&rect3.right);
	x1 = rect.left;
	y1 = rect.top;
	OffsetRect(&rect, -x1, -y1);
	OffsetRect(&rect3, -x1, -y1);
	if (rect.bottom - border * 2 < 0)
		border = 1;
	if (rect.right - border * 2 < 0)
		border = 1;
	hDC = GetWindowDC(hWND);
	if (hDC != NULL)
	{
		SetRect(&rect2, 0, 0, rect.right, border);
		InvertRect(hDC, &rect2);
		SetRect(&rect2, 0, border, border, rect.bottom);
		InvertRect(hDC, &rect2);
		SetRect(&rect2, border, rect.bottom - border, rect.right, rect.bottom);
		InvertRect(hDC, &rect2);
		SetRect(&rect2, rect.right - border, border, rect.right, rect.bottom - border);
		InvertRect(hDC, &rect2);
		ReleaseDC(hWND, hDC);
		return TRUE;
	}
	return FALSE;
}


