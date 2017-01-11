#pragma once
#include "TinyButton.h"

namespace TinyUI
{
	class TinyCheckBox : public TinyButton
	{
		DECLARE_DYNAMIC(TinyCheckBox)
	public:
		TinyCheckBox();
		virtual ~TinyCheckBox();
		virtual LPCSTR RetrieveTitle();
		virtual DWORD RetrieveStyle();
	};
}



