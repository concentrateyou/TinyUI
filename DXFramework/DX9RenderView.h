#pragma once
#include "DX9Texture2D.h"

namespace DXFramework
{
	class DX9RenderView
	{
		DISALLOW_COPY_AND_ASSIGN(DX9RenderView)
	public:
		DX9RenderView(DX9& dx9);
		virtual ~DX9RenderView();
		DX9&		GetDX9();
		TinySize	GetSize() const;
		BOOL		Create();
		BOOL		Create(INT cx, INT cy);
		void		Destory();
		BOOL		Resize();
		BOOL		Resize(INT cx, INT cy);
		IDirect3DSurface9*		GetSurface() const;
	public:
		BOOL		BeginDraw();
		BOOL		EndDraw();
	protected:
		DX9&							m_dx9;
		TinySize						m_size;
		TinyComPtr<IDirect3DSurface9>	m_render2D;
	};
}


