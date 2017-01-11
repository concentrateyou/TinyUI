#include "../stdafx.h"
#include "TinyRadioButton.h"

namespace TinyUI
{
	TinyRadioButton::TinyRadioButton()
	{
	};
	TinyRadioButton::~TinyRadioButton()
	{

	};
	DWORD TinyRadioButton::RetrieveStyle()
	{
		return (WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_NOTIFY | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	}
	LPCSTR TinyRadioButton::RetrieveTitle()
	{
		return TEXT("TinyRadioButton");
	};
}

