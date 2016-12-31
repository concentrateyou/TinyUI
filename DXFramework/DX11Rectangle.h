#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11TextureShader.h"

namespace DXFramework
{
	class DX11Rectangle
	{
		struct VERTEXTYPE
		{
			D3DXVECTOR3 position;
			D3DXVECTOR4 color;
		};
		DISALLOW_COPY_AND_ASSIGN(DX11Rectangle)
	public:
		DX11Rectangle();
		virtual ~DX11Rectangle();
		BOOL Create(const DX11& dx11, INT cx, INT cy);
		void SetPosition(INT x, INT y);
	};
}

