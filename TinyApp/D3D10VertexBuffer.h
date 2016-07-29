#pragma once
using namespace TinyUI;

namespace D3D
{
	class CD3D10VertexBuffer
	{
		friend class CD3D10Device;
	public:
		CD3D10VertexBuffer();
		~CD3D10VertexBuffer();
		BOOL Create(CD3D10Device* device, BOOL bStatic);
		BOOL Flush();
	private:
		TinyComPtr<ID3D10Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D10Buffer>	m_textureVertexBuffer;
		ID3D10Buffer*				m_buffers;
		D3DXVECTOR4					m_vertexs[4];
		D3DXVECTOR2					m_textureVertexs[4];
	};
}

