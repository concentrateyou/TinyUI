#pragma once
#include "DX10ImageElement2D.h"
#include "DX10Texture2D.h"
#include "DX10TextureShader.h"
#include "Render/TinyGDI.h"
using namespace TinyFramework;

namespace DXFramework
{
	class DX10Image2D : public DX10ImageElement2D, public DX10Texture2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX10Image2D)
		DISALLOW_COPY_AND_ASSIGN(DX10Image2D)
	public:
		DX10Image2D();
		virtual ~DX10Image2D();
	public:
		BOOL		Create(DX10& dx10, D3D10_TEXTURE2D_DESC& desc) OVERRIDE;
		BOOL		Create(DX10& dx10, ID3D10Texture2D* texture2D) OVERRIDE;
		BOOL		Create(DX10& dx10, INT cx, INT cy, const BYTE* bits, BOOL bReadoly) OVERRIDE;
		BOOL		Create(DX10& dx10, INT cx, INT cy) OVERRIDE;
		BOOL		Load(DX10& dx10, const BYTE* bits, LONG dwSize) OVERRIDE;
		BOOL		Load(DX10& dx10, HANDLE hResource) OVERRIDE;
		BOOL		Load(DX10& dx10, const CHAR* pzFile) OVERRIDE;
		void		Destory() OVERRIDE;
		BOOL		DrawImage(DX10& dx10);
	public:
		BOOL		Copy(DX10& dx10, D3D10_BOX* ps, const BYTE* bits, LONG size);
		BOOL		Copy(DX10& dx10, const BYTE* bits, LONG size, UINT stride);
		BOOL		BitBlt(DX10& dx10, const BYTE* bits, LONG size, LONG linesize);
		BOOL		BitBlt(DX10& dx10, const TinyRectangle& dst, HBITMAP hBitmapSrc, const TinyPoint& src);
		BOOL		BitBlt(DX10& dx10, const TinyRectangle& dst, HDC hDCSrc, const TinyPoint& src);
	private:
		BOOL		Initialize(DX10& dx10);
		BOOL		Calculate(DX10& dx10);
	protected:
		VERTEXTYPE					m_vertexTypes[6];
		TinyComPtr<ID3D10Buffer>	m_vertex;
		TinyComPtr<ID3D10Buffer>	m_index;
	};
}

