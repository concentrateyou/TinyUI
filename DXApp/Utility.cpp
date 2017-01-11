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

BOOL WINAPI InvertWindow(HWND hWND)
{
	if (!hWND)
		return FALSE;
	TinyRectangle rectangle;
	GetWindowRect(hWND, &rectangle);
	rectangle.OffsetRect(-rectangle.Position().x, -rectangle.Position().y);
	HDC hDC = GetWindowDC(hWND);
	if (hDC != NULL)
	{
		RECT s = { 0 };
		SetRect(&s, 0, 0, rectangle.right, 4);
		InvertRect(hDC, &s);
		SetRect(&s, 0, 4, 4, rectangle.bottom);
		InvertRect(hDC, &s);
		SetRect(&s, 4, rectangle.bottom - 4, rectangle.right, rectangle.bottom);
		InvertRect(hDC, &s);
		SetRect(&s, rectangle.right - 4, 4, rectangle.right, rectangle.bottom - 4);
		InvertRect(hDC, &s);
		ReleaseDC(hWND, hDC);
		return TRUE;
	}
	return FALSE;
}


