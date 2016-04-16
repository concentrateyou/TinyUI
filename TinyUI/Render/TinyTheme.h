#pragma once
#include "../Common/TinyCommon.h"
#include "../Render/TinyTheme.h"
#include <vssym32.h>
#include <uxtheme.h>

#pragma comment(lib, "UxTheme.lib")

namespace TinyUI
{
	class TinyTheme
	{
	public:
		TinyTheme(HWND hWND);
		~TinyTheme();
		BOOL Open(LPCWSTR pszClassList);
		void Close();
	private:
		HTHEME  m_hTheme;
		HWND	m_hWND;
	};
}