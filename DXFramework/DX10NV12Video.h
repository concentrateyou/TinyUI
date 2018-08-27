#pragma once
#include "DX10Image2D.h"
#include "IO/TinyIO.h"
#include "Render/TinyGDI.h"
using namespace TinyFramework;
using namespace TinyFramework::IO;

namespace DXFramework
{
	class DX10NV12Video : public DX10Element2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX10NV12Video)
		DISALLOW_COPY_AND_ASSIGN(DX10NV12Video)
	public:
		DX10NV12Video();
		virtual ~DX10NV12Video();
		BOOL			IsEmpty() const;
		BOOL			Create(DX10& dx10, INT cx, INT cy);
		void			Destory();
		BOOL			Copy(DX10& dx10, const BYTE* pY, UINT strideY, const BYTE* pUV, UINT strideUV);
		DX10Texture2D*	GetTextureY();
		DX10Texture2D*	GetTextureNV();
	public:
		BOOL			DrawImage(DX10& dx10) OVERRIDE;
	private:
		BOOL			Initialize(DX10& dx10);
		BOOL			Calculate(DX10& dx10);
	private:
		DX10Texture2D				m_textureY;
		DX10Texture2D				m_textureNV;
		VERTEXTYPE					m_vertexTypes[6];
		TinyComPtr<ID3D10Buffer>	m_vertex;
		TinyComPtr<ID3D10Buffer>	m_index;
	};
}


