#pragma once
#include "DX9Texture2D.h"

namespace DXFramework
{
	class DX9RenderView
	{
	public:
		DX9RenderView(DX9& dx9);
		virtual ~DX9RenderView();
		DX9&		GetDX9();
		TinySize	GetSize() const;
	public:
		BOOL		BeginDraw();
		BOOL		EndDraw();
	protected:
		HANDLE					m_handle;
		DX9&					m_dx9;
		TinySize				m_size;
		TinyComPtr<ID3DXSprite>	m_sprite;
	};
}


