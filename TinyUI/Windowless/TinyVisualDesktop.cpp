#include "../stdafx.h"
#include "TinyVisualDesktop.h"

namespace TinyUI
{
	namespace Windowless
	{

		LPCSTR TinyVisualDesktop::RetrieveTag()
		{
			return TEXT("window");
		}

		HRESULT TinyVisualDesktop::OnDraw(TinyDC& dc, TinyRectangle& drawRect)
		{
			return FALSE;
		}

		HRESULT TinyVisualDesktop::OnMouseMove(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualDesktop::OnLButtonDown(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualDesktop::OnLButtonUp(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualDesktop::OnRButtonDown(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualDesktop::OnRButtonUp(POINT pos)
		{
			return FALSE;
		}

	}
}
