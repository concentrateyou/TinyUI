#include "../stdafx.h"
#include "TinyVisualTree.h"
#include "TinyVisualManage.h"
#include "TinyVisualCaption.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualSysButton::TinyVisualSysButton(TinyVisual* spvisParent, TinyVisualTree* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualSysButton::~TinyVisualSysButton()
		{

		}
		TinyString TinyVisualSysButton::RetrieveTag() const
		{
			return TinyVisualTag::SYSBUTTON;
		}
		BOOL TinyVisualSysButton::OnDraw(HDC hDC)
		{
			TinyImage& image = m_images[NORMAL];
			if (image.IsEmpty()) 
				return FALSE;
			TinyClipCanvas canvas(hDC, this);
			TinyRectangle clip = canvas.GetClipBox();
			canvas.DrawImage(image, clip, 0, 0, image.GetSize().cx, image.GetSize().cy);
			return TRUE;
		}
		HRESULT TinyVisualSysButton::OnMouseMove(TinyPoint pos, DWORD dwFlags)
		{
			return FALSE;
		}

		HRESULT TinyVisualSysButton::OnLButtonDown(TinyPoint pos, DWORD dwFlags)
		{
			return FALSE;
		}

		HRESULT TinyVisualSysButton::OnLButtonUp(TinyPoint pos, DWORD dwFlags)
		{
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyVisualCaption::TinyVisualCaption(TinyVisual* spvisParent, TinyVisualTree* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualCaption::~TinyVisualCaption()
		{

		}
		TinyString TinyVisualCaption::RetrieveTag() const
		{
			return TinyVisualTag::CAPTION;
		}
	}
}
