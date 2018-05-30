#pragma once
#include "TinyButton.h"

namespace TinyFramework
{
	class TinyRadioButton : public TinyButton
	{
		DECLARE_DYNAMIC(TinyRadioButton)
	public:
		TinyRadioButton();
		virtual ~TinyRadioButton();
		virtual LPCSTR RetrieveTitle();
		virtual DWORD RetrieveStyle();
	};
}



