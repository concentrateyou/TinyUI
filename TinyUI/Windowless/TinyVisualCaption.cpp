#include "../stdafx.h"
#include "TinyVisualCaption.h"

namespace TinyUI
{
	namespace Windowless
	{

		LPCSTR TinySysButton::RetrieveTag()
		{
			return TEXT("sysbutton");
		}
		BOOL TinySysButton::ParseVisual(TiXmlElement* ps)
		{
			return FALSE;
		}
		HRESULT TinySysButton::OnDraw(TinyDC& dc, TinyRectangle& drawRect)
		{
			return FALSE;
		}

		HRESULT TinySysButton::OnMouseMove(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinySysButton::OnLButtonDown(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinySysButton::OnLButtonUp(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinySysButton::OnRButtonDown(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinySysButton::OnRButtonUp(POINT pos)
		{
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		LPCSTR TinyVisualCaption::RetrieveTag()
		{
			return TEXT("caption");
		}
	}
}
