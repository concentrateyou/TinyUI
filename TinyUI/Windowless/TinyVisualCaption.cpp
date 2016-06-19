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
		HRESULT TinyVisualSysButton::OnDraw(HDC hDC)
		{
			TinyImage& image = m_images[NORMAL];
			if (image.IsEmpty()) return S_FALSE;
			TinyCanvas canvas(hDC);
			TinyCanvasClip clip(canvas, this);
			TinyRectangle rcClip = clip.GetClipBox();
			canvas.DrawImage(image, rcClip, 0, 0, image.GetSize().cx, image.GetSize().cy);
			return S_OK;
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
		void TinyVisualCaption::Resize()
		{
			TinyVisual* spvisParent = m_spvisParent;
		}
		HRESULT TinyVisualCaption::OnDraw(HDC hDC)
		{
			return S_OK;
		}
		HRESULT TinyVisualCaption::OnMouseMove(TinyPoint pos, DWORD dwFlags)
		{
			return FALSE;
		}
		HRESULT TinyVisualCaption::OnLButtonDown(TinyPoint pos, DWORD dwFlags)
		{
			return FALSE;
		}
		HRESULT TinyVisualCaption::OnLButtonUp(TinyPoint pos, DWORD dwFlags)
		{
			return FALSE;
		}
	}
}
