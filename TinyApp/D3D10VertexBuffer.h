#pragma once
#include "D3D10Device.h"
using namespace TinyUI;

namespace D3D
{
	class CD3D10VertexBuffer
	{
		friend class CD3D10Device;
	public:
		CD3D10VertexBuffer();
		~CD3D10VertexBuffer();
		BOOL Create(BOOL bStatic);
	private:
		TinyComPtr<ID3D10Buffer> m_vertexBuffer;
	};
}

