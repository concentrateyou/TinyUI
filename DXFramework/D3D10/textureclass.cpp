#include "stdafx.h"
#include "textureclass.h"

TextureClass::TextureClass()
{
	m_texture = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(ID3D10Device* device, CHAR* filename)
{
	HRESULT result;


	// Load the texture in.
	result = D3DX10CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool TextureClass::Initialize(ID3D10Device* device, HANDLE hResource)
{
	HRESULT result;
	TinyComPtr<ID3D10Resource> resource;
	if (FAILED(result = device->OpenSharedResource(hResource, __uuidof(ID3D10Resource), (void**)&resource)))
		return false;
	TinyComPtr<ID3D10Texture2D> texture2D;
	if (FAILED(result = resource->QueryInterface(__uuidof(ID3D10Texture2D), (void**)&texture2D)))
	{
		return false;
	}
	D3D10_TEXTURE2D_DESC dtd;
	texture2D->GetDesc(&dtd);
	D3D10_SHADER_RESOURCE_VIEW_DESC dsrvd;
	::ZeroMemory(&dsrvd, sizeof(dsrvd));
	dsrvd.Format = dtd.Format;
	dsrvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	dsrvd.Texture2D.MipLevels = 1;
	if (FAILED(result = device->CreateShaderResourceView(texture2D, &dsrvd, &m_texture)))
	{
		return false;
	}
	return true;
}

bool TextureClass::Initialize(ID3D10Device*device, void* data, SIZE_T size)
{
	HRESULT result;


	// Load the texture in.
	result = D3DX10CreateShaderResourceViewFromMemory(device, data, size, NULL, NULL, &m_texture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture resource.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}


ID3D10ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}