#include "../stdafx.h"
#include "TinyCheckBox.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyCheckBox, TinyControl);
	TinyCheckBox::TinyCheckBox()
	{
	};
	TinyCheckBox::~TinyCheckBox()
	{

	};
	DWORD TinyCheckBox::RetrieveStyle()
	{
		return (WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_NOTIFY | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_CHECKBOX);
	}
	LPCSTR TinyCheckBox::RetrieveTitle()
	{
		return TEXT("TinyCheckBox");
	};
}

