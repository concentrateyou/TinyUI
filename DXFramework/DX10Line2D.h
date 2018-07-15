#pragma once
#include "DX10Element2D.h"
#include "DX10ColorShader.h"
using namespace TinyFramework;

namespace DXFramework
{
	class DX10Line2D : public DX10Element2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX10Line2D)
		DISALLOW_COPY_AND_ASSIGN(DX10Line2D)
	public:
		DX10Line2D();
		virtual	~DX10Line2D();
		BOOL	Create(DX10& dx10);
		void	Destory();
		DWORD	GetIndexs() const;
		BOOL	DrawLine(DX10& dx10, const XMFLOAT2* points, DWORD count, const XMFLOAT4& color);
	protected:
		TinyBuffer<VERTEXTYPE>		m_vertexTypes;
		TinyComPtr<ID3D10Buffer>	m_vertex;
		TinyComPtr<ID3D10Buffer>	m_index;
	};
}

