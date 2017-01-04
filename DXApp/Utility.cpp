#include "stdafx.h"
#include "Utility.h"

BOOL  WINAPI GetModuleList(HANDLE hProcess, TinyArray<TinyString> &moduleList)
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

