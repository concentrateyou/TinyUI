#pragma once
#include "textureclass.h"
#include "../DXFramework.h"

class BitmapClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
	};

public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D10Device*, int, int, CHAR*, int, int);
	bool Initialize(ID3D10Device*, int, int, HANDLE hResource, int, int);
	bool Initialize(ID3D10Device* device, int screenWidth, int screenHeight, BYTE* data, int bitmapWidth, int bitmapHeight);
	void Shutdown();
	bool Render(ID3D10Device*, int, int);

	int GetIndexCount();
	ID3D10ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D10Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(int, int);
	void RenderBuffers(ID3D10Device*);

	bool LoadTexture(ID3D10Device*, CHAR*);
	bool LoadTexture(ID3D10Device*, BYTE*);
	bool LoadTexture(ID3D10Device* device, HANDLE hResource);
	void ReleaseTexture();

private:
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};