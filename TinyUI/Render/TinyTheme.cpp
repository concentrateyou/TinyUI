#include "../stdafx.h"
#include "TinyTheme.h"

namespace TinyUI
{
	TinyTheme::TinyTheme(HWND hWND)
		:m_hTheme(NULL),
		m_hWND(hWND)
	{

	}
	TinyTheme::~TinyTheme()
	{

	}
	BOOL TinyTheme::Open(LPCWSTR pszClassList)
	{
		return (m_hTheme = OpenThemeData(m_hWND, pszClassList)) != NULL;
	}
	void TinyTheme::Close()
	{
		CloseThemeData(m_hTheme);
		m_hTheme = NULL;
	}
}
