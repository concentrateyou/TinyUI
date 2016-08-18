#pragma oce
#include "../DXFramework.h"

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D10Device*device, CHAR* pzFile);
	bool Initialize(ID3D10Device*device, HANDLE hResource);
	bool Initialize(ID3D10Device*device, void* data, SIZE_T size);
	void Shutdown();

	ID3D10ShaderResourceView* GetTexture();

private:
	ID3D10ShaderResourceView* m_texture;
};