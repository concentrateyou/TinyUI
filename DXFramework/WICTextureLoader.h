#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	enum WIC_LOADER_FLAGS
	{
		WIC_LOADER_DEFAULT = 0,
		WIC_LOADER_FORCE_SRGB = 0x1,
		WIC_LOADER_IGNORE_SRGB = 0x2,
	};

	HRESULT __cdecl CreateWICTextureFromMemory(
		ID3D11Device* d3dDevice,
		const BYTE* wicData,
		size_t wicDataSize,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize = 0);

	HRESULT __cdecl CreateWICTextureFromFile(
		ID3D11Device* d3dDevice,
		const WCHAR* szFileName,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize = 0);

	HRESULT __cdecl CreateWICTextureFromMemory(
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		const BYTE* wicData,
		size_t wicDataSize,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize = 0);

	HRESULT __cdecl CreateWICTextureFromFile(
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		const WCHAR* szFileName,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize = 0);

	HRESULT __cdecl CreateWICTextureFromMemoryEx(
		ID3D11Device* d3dDevice,
		const BYTE* wicData,
		size_t wicDataSize,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView);

	HRESULT __cdecl CreateWICTextureFromFileEx(
		ID3D11Device* d3dDevice,
		const WCHAR* szFileName,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView);

	HRESULT __cdecl CreateWICTextureFromMemoryEx(
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		const BYTE* wicData,
		size_t wicDataSize,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView);

	HRESULT __cdecl CreateWICTextureFromFileEx(
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		const WCHAR* szFileName,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView);

}


