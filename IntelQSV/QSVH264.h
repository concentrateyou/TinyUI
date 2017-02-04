#pragma once
#include "Common/TinyCommon.h"
#include <mfxsession.h>
#include <TlHelp32.h>
#include <string>
using namespace std;
using namespace TinyUI;

namespace QSV
{
	class QSVH264
	{
	public:
		QSVH264();
		~QSVH264();
		BOOL Initialize();
	private:
		mfxSession	m_session;
	};
}



