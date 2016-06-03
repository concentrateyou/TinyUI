#include "../stdafx.h"
#include "TinyTheme.h"

namespace TinyUI
{
	TinyTheme::TinyTheme()
		:m_hThemeDLL(NULL)
	{
		m_hThemeDLL = LoadLibrary(_T("UXTHEME.DLL"));
	}
	TinyTheme::~TinyTheme()
	{
		if (m_hThemeDLL)
		{
			FreeLibrary(m_hThemeDLL);
			m_hThemeDLL = NULL;
		}
	}
	void TinyTheme::EnumThemeFunctions()
	{

	}
}
