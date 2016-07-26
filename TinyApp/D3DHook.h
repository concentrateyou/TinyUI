#pragma once
#include <Windows.h>

namespace D3D
{
	BOOL WINAPI InjectLibrary(HANDLE hProcess, const CHAR *pszDLL);
}
