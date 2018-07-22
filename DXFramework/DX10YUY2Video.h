#pragma once
#include "DX10Texture2DElement.h"
#include "Render/TinyGDI.h"
using namespace TinyFramework;

namespace DXFramework
{
	/// <summary>
	/// YUY2 (packet)
	/// </summary>
	class DX10YUY2Video : public DX10Texture2DElement
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX10YUY2Video)
		DISALLOW_COPY_AND_ASSIGN(DX10YUY2Video)
	public:
		DX10YUY2Video();
		virtual ~DX10YUY2Video();
		BOOL			IsEmpty() const;
		BOOL			Create(DX10& dx10, INT cx, INT cy);
		void			Destory();
		BOOL			Copy(DX10& dx10, const BYTE* bits, UINT stride);
		DX10Texture2D*	GetTexture();
	public:
		BOOL			DrawImage(DX10& dx10) OVERRIDE;
	private:
		BOOL			Initialize(DX10& dx10);
		BOOL			Calculate(DX10& dx10);
	private:
		DX10Texture2D				m_texture;
		VERTEXTYPE					m_vertexTypes[6];
		TinyComPtr<ID3D10Buffer>	m_vertex;
		TinyComPtr<ID3D10Buffer>	m_index;
	};
}


