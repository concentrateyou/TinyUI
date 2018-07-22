#pragma once
#include "DX10Element.h"
#include "DX10ColorShader.h"
using namespace TinyFramework;

namespace DXFramework
{
	class DX10Rectangle2D : public DX10Element
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX10Rectangle2D)
		DISALLOW_COPY_AND_ASSIGN(DX10Rectangle2D)
	public:
		DX10Rectangle2D();
		virtual	~DX10Rectangle2D();
		BOOL	Create(DX10& dx10);
		void	Destory();
		DWORD	GetIndexs() const;
	public:
		BOOL	DrawRectangle(DX10& dx10, const XMFLOAT2 points[4], const XMFLOAT4& color);
		BOOL	FillRectangle(DX10& dx10, const XMFLOAT2 points[4], const XMFLOAT4& color);
	protected:
		DWORD						m_index;
		XMFLOAT2					m_points[4];
		VERTEXTYPE					m_vertexTypes[4];
		TinyComPtr<ID3D10Buffer>	m_vertex;
		TinyComPtr<ID3D10Buffer>	m_indexs[2];
	};
}

