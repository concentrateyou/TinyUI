#pragma once
#include "DX11ImageElement2D.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11ImageNV12BT601 : public DX11ImageElement2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX11ImageNV12BT601)
	public:
		DX11ImageNV12BT601();
		virtual ~DX11ImageNV12BT601();
		BOOL Create(DX11& dx11, INT cx, INT cy);
		void Destory();
		BOOL Copy(DX11& dx11,const BYTE* pY, UINT strideY, const BYTE* pUV, UINT strideUV);
	private:
		DX11Texture2D	m_textureY;
		DX11Texture2D	m_textureNV;
	};
}


