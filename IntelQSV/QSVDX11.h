#pragma once
#include "Common/TinyCommon.h"
#include <d3d11_1.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <mfxcommon.h>
#include <mfxsession.h>
#include <mfxstructures.h>
using namespace TinyUI;

namespace QSV
{
	class QSVDX11
	{
	public:
		QSVDX11();
		~QSVDX11();
		BOOL Initialize(HWND hWND);
	private:
		mfxSession					m_session;
		TinyComPtr<IDXGIFactory2>   m_dxgiFactory2;
	};
}



