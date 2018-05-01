#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11Image2D.h"
#include "DX11Duplicator.h"
using namespace TinyUI::IO;

namespace DXFramework
{
	class DX11DuplicatorTask : public TinyThread
	{
	public:
		DX11DuplicatorTask(DX11& dx11, DX11Image2D& image);
		virtual ~DX11DuplicatorTask();
		BOOL					Submit();
		BOOL					Close(DWORD dwMS = INFINITE) OVERRIDE;
	private:
		void					OnMessagePump();
	private:
		DX11&					m_dx11;
		TinyEvent				m_close;
		DX11Image2D&			m_image;
	};
}


