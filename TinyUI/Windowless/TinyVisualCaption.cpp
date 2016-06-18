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
		HRESULT TinyVisualSysButton::OnDraw(TinyCanvas& canvas)
		{
			TinyImage& image = m_images[NORMAL];
			if (image.IsEmpty()) return S_FALSE;
			canvas.DrawImage(image, m_windowRect, 0, 0, image.GetSize().cx, image.GetSize().cy);
			return S_OK;
		}
		HRESULT TinyVisualSysButton::OnMouseMove(const TinyPoint& pos, DWORD dwKey)
		{
			return FALSE;
		}

		HRESULT TinyVisualSysButton::OnLButtonDown(const TinyPoint& pos, DWORD dwKey)
		{
			TRACE("SysButton-OnLButtonDown:%d,%d\n", pos.x, pos.y);
			return FALSE;
		}

		HRESULT TinyVisualSysButton::OnLButtonUp(const TinyPoint& pos, DWORD dwKey)
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
		HRESULT TinyVisualCaption::OnDraw(TinyCanvas& canvas)
		{
			TinyVisual* spvis = m_vtree->GetVisual(this, CMD_CHILD);
			while (spvis)
			{
				spvis->OnDraw(canvas);
				spvis = m_vtree->GetVisual(spvis, CMD_NEXT);
			}
			return S_OK;
		}
		HRESULT TinyVisualCaption::OnMouseMove(const TinyPoint& pos, DWORD dwKey)
		{
			return FALSE;
		}
		HRESULT TinyVisualCaption::OnLButtonDown(const TinyPoint& pos, DWORD dwKey)
		{
			return FALSE;
		}
		HRESULT TinyVisualCaption::OnLButtonUp(const TinyPoint& pos, DWORD dwKey)
		{
			return FALSE;
		}
	}
}
