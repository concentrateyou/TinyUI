#pragma once
#include "Control/TinyControl.h"

#include "DX10Image2D.h"
#include "DX10Rectangle2D.h"
#include "DX10Line2D.h"
#include "DX10Graphics2D.h"

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
		DX10			m_dx11;
		DX10Graphics2D	m_graphics;
		DX10Image2D		m_image2D;

		DX10Rectangle2D	m_rectangle2D;
	};
}


