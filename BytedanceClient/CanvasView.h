#pragma once
#include "Control/TinyControl.h"

#include "DX9Image2D.h"
#include "DX9Graphics2D.h"

using namespace TinyFramework;
using namespace DXFramework;

namespace Bytedance
{
	class CanvasView : public TinyControl
	{
		DECLARE_DYNAMIC(CanvasView)
	public:
		CanvasView();
		virtual ~CanvasView();
		LPCSTR	RetrieveClassName() OVERRIDE;
		LPCSTR	RetrieveTitle() OVERRIDE;
		DWORD	RetrieveStyle() OVERRIDE;
		DWORD	RetrieveExStyle() OVERRIDE;
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		DX9						m_dx9;
		DX9Graphics2D			m_graphics;
		DX9Image2D				m_image2D;
	};
}

