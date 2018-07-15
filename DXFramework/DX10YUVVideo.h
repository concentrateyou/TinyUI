#pragma once
#include "DX10ImageElement2D.h"
#include "Render/TinyGDI.h"
using namespace TinyFramework;

namespace DXFramework
{
	class DX10YUVVideo : public DX10ImageElement2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX10YUVVideo)
		DISALLOW_COPY_AND_ASSIGN(DX10YUVVideo)
	public:
		DX10YUVVideo();
		virtual ~DX10YUVVideo();
		BOOL			IsEmpty() const;
		BOOL			Create(DX10& dx10, INT cx, INT cy);
		void			Destory();
		BOOL			DrawImage(DX10& dx10);
		BOOL			Copy(DX10& dx10, const BYTE* pY, UINT strideY, const BYTE* pU, UINT strideU, const BYTE* pV, UINT strideV);
		DX10Texture2D*	GetTextureY();
		DX10Texture2D*	GetTextureU();
		DX10Texture2D*	GetTextureV();
	private:
		BOOL			Initialize(DX10& dx10);
		BOOL			Calculate(DX10& dx10);
	private:
		DX10Texture2D				m_textureY;
		DX10Texture2D				m_textureU;
		DX10Texture2D				m_textureV;
		VERTEXTYPE					m_vertexTypes[6];
		TinyComPtr<ID3D10Buffer>	m_vertex;
		TinyComPtr<ID3D10Buffer>	m_index;
	};
}


