#include "../stdafx.h"
#include "TinyVisualCaption.h"
#include "TinyVisualManage.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinySysButton::TinySysButton(TinyVisual* spvisParent)
			:TinyVisual(spvisParent)
		{

		}
		TinySysButton::~TinySysButton()
		{

		}
		LPCSTR TinySysButton::RetrieveTag()
		{
			return TinyVisualTag::SYSBUTTON;
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
		TinyVisualCaption::TinyVisualCaption(TinyVisual* spvisParent)
			:TinyVisual(spvisParent)
		{

		}
		TinyVisualCaption::~TinyVisualCaption()
		{

		}
		LPCSTR TinyVisualCaption::RetrieveTag()
		{
			return TinyVisualTag::CAPTION;
		}
		HRESULT TinyVisualCaption::OnDraw(TinyDC& dc, TinyRectangle& drawRect)
		{
			return FALSE;
		}
		HRESULT TinyVisualCaption::OnMouseMove(POINT pos)
		{
			return FALSE;
		}
		HRESULT TinyVisualCaption::OnLButtonDown(POINT pos)
		{
			return FALSE;
		}
		HRESULT TinyVisualCaption::OnLButtonUp(POINT pos)
		{
			return FALSE;
		}
		HRESULT TinyVisualCaption::OnRButtonDown(POINT pos)
		{
			return FALSE;
		}
		HRESULT TinyVisualCaption::OnRButtonUp(POINT pos)
		{
			return FALSE;
		}
	}
}
