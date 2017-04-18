#include "stdafx.h"
#include "WICTexture.h"
#include <d2d1.h>
#include <Wincodec.h>
#pragma comment(lib,"Windowscodecs.lib")

namespace DXFramework
{
	BOOL g_WIC2 = FALSE;
	IWICImagingFactory* GetWIC()
	{
		static INIT_ONCE s_initOnce = INIT_ONCE_STATIC_INIT;
		IWICImagingFactory* factory = nullptr;
		InitOnceExecuteOnce(&s_initOnce,
			[](PINIT_ONCE, PVOID, PVOID *factory) -> BOOL
		{
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
			HRESULT hRes = CoCreateInstance(
				CLSID_WICImagingFactory2,
				nullptr,
				CLSCTX_INPROC_SERVER,
				__uuidof(IWICImagingFactory2),
				factory
			);

			if (SUCCEEDED(hRes))
			{
				g_WIC2 = TRUE;
				return TRUE;
			}
			else
			{
				hRes = CoCreateInstance(
					CLSID_WICImagingFactory1,
					nullptr,
					CLSCTX_INPROC_SERVER,
					__uuidof(IWICImagingFactory),
					factory
				);
				return SUCCEEDED(hRes) ? TRUE : FALSE;
			}
#else
			return SUCCEEDED(CoCreateInstance(
				CLSID_WICImagingFactory,
				nullptr,
				CLSCTX_INPROC_SERVER,
				__uuidof(IWICImagingFactory),
				factory)) ? TRUE : FALSE;
#endif
		}, NULL, reinterpret_cast<LPVOID*>(&factory));
		return factory;
	}

	BOOL IsWIC2()
	{
		return g_WIC2;
	}

	struct WICConvert
	{
		GUID        source;
		GUID        target;
	};

	const WICConvert g_WICConvert[] =
	{
		{ GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM
		{ GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
		{ GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
		{ GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT 
		{ GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT 
		{ GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM
		{ GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM
		{ GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat32bppRGBE,             GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
		{ GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
#endif																
	};
	struct WICTranslate
	{
		GUID                wic;
		DXGI_FORMAT         format;
	};
	const WICTranslate g_WICFormats[] =
	{
		{ GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },

		{ GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
		{ GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },

		{ GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
		{ GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM }, // DXGI 1.1
		{ GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM }, // DXGI 1.1

		{ GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM }, // DXGI 1.1
		{ GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },

		{ GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
		{ GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },

		{ GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
		{ GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
		{ GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
		{ GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },

		{ GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },
	};
	DXGI_FORMAT WICToDXGI(const GUID& guid)
	{
		for (size_t i = 0; i < _countof(g_WICFormats); ++i)
		{
			if (memcmp(&g_WICFormats[i].wic, &guid, sizeof(GUID)) == 0)
				return g_WICFormats[i].format;
		}

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
		if (g_WIC2)
		{
			if (memcmp(&GUID_WICPixelFormat96bppRGBFloat, &guid, sizeof(GUID)) == 0)
				return DXGI_FORMAT_R32G32B32_FLOAT;
		}
#endif
		return DXGI_FORMAT_UNKNOWN;
	}
	size_t WICBitsPerPixel(REFGUID targetGuid)
	{
		auto pWIC = GetWIC();
		if (!pWIC)
			return 0;
		TinyComPtr<IWICComponentInfo> cinfo;
		if (FAILED(pWIC->CreateComponentInfo(targetGuid, &cinfo)))
			return 0;
		WICComponentType type;
		if (FAILED(cinfo->GetComponentType(&type)))
			return 0;
		if (type != WICPixelFormat)
			return 0;
		TinyComPtr<IWICPixelFormatInfo> pfinfo;
		if (FAILED(cinfo.QueryInterface(pfinfo)))
			return 0;
		UINT bpp;
		if (FAILED(pfinfo->GetBitsPerPixel(&bpp)))
			return 0;
		return bpp;
	}
	inline DXGI_FORMAT MakeSRGB(_In_ DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		case DXGI_FORMAT_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM_SRGB;

		case DXGI_FORMAT_BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM_SRGB;

		case DXGI_FORMAT_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM_SRGB;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

		case DXGI_FORMAT_BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM_SRGB;

		default:
			return format;
		}
	}
	HRESULT CreateTextureFromWIC(ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		IWICBitmapFrameDecode *frame,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView)
	{
		UINT width, height;
		HRESULT hRes = frame->GetSize(&width, &height);
		if (hRes != S_OK)
			return hRes;
		ASSERT(width > 0 && height > 0);
		if (!maxsize)
		{
			switch (d3dDevice->GetFeatureLevel())
			{
			case D3D_FEATURE_LEVEL_9_1:
			case D3D_FEATURE_LEVEL_9_2:
				maxsize = 2048 /*D3D_FL9_1_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
				break;
			case D3D_FEATURE_LEVEL_9_3:
				maxsize = 4096 /*D3D_FL9_3_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
				break;
			case D3D_FEATURE_LEVEL_10_0:
			case D3D_FEATURE_LEVEL_10_1:
				maxsize = 8192 /*D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
				break;
			default:
				maxsize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
				break;
			}
		}
		ASSERT(maxsize > 0);
		UINT twidth, theight;
		if (width > maxsize || height > maxsize)
		{
			float ar = static_cast<float>(height) / static_cast<float>(width);
			if (width > height)
			{
				twidth = static_cast<UINT>(maxsize);
				theight = std::max<UINT>(1, static_cast<UINT>(static_cast<float>(maxsize) * ar));
			}
			else
			{
				theight = static_cast<UINT>(maxsize);
				twidth = std::max<UINT>(1, static_cast<UINT>(static_cast<float>(maxsize) / ar));
			}
			ASSERT(twidth <= maxsize && theight <= maxsize);
		}
		else
		{
			twidth = width;
			theight = height;
		}
		WICPixelFormatGUID pixelFormat;
		hRes = frame->GetPixelFormat(&pixelFormat);
		if (hRes != S_OK)
			return hRes;
		WICPixelFormatGUID convertGUID;
		memcpy(&convertGUID, &pixelFormat, sizeof(WICPixelFormatGUID));
		size_t bpp = 0;
		DXGI_FORMAT format = WICToDXGI(pixelFormat);
		if (format == DXGI_FORMAT_UNKNOWN)
		{
			if (memcmp(&GUID_WICPixelFormat96bppRGBFixedPoint, &pixelFormat, sizeof(WICPixelFormatGUID)) == 0)
			{
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
				if (g_WIC2)
				{
					memcpy(&convertGUID, &GUID_WICPixelFormat96bppRGBFloat, sizeof(WICPixelFormatGUID));
					format = DXGI_FORMAT_R32G32B32_FLOAT;
					bpp = 96;
				}
				else
#endif
				{
					memcpy(&convertGUID, &GUID_WICPixelFormat128bppRGBAFloat, sizeof(WICPixelFormatGUID));
					format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					bpp = 128;
				}
			}
			else
			{
				for (size_t i = 0; i < _countof(g_WICConvert); ++i)
				{
					if (memcmp(&g_WICConvert[i].source, &pixelFormat, sizeof(WICPixelFormatGUID)) == 0)
					{
						memcpy(&convertGUID, &g_WICConvert[i].target, sizeof(WICPixelFormatGUID));
						format = WICToDXGI(g_WICConvert[i].target);
						ASSERT(format != DXGI_FORMAT_UNKNOWN);
						bpp = WICBitsPerPixel(convertGUID);
						break;
					}
				}
			}

			if (format == DXGI_FORMAT_UNKNOWN)
				return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}
		else
		{
			bpp = WICBitsPerPixel(pixelFormat);
		}
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
		if ((format == DXGI_FORMAT_R32G32B32_FLOAT) && d3dContext != 0 && textureView != 0)
		{
			UINT fmtSupport = 0;
			hRes = d3dDevice->CheckFormatSupport(DXGI_FORMAT_R32G32B32_FLOAT, &fmtSupport);
			if (FAILED(hRes) || !(fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
			{
				memcpy(&convertGUID, &GUID_WICPixelFormat128bppRGBAFloat, sizeof(WICPixelFormatGUID));
				format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				bpp = 128;
			}
		}
#endif
		if (!bpp)
			return E_FAIL;
		if (loadFlags & WIC_LOADER_FORCE_SRGB)
		{
			format = MakeSRGB(format);
		}
		else if (!(loadFlags & WIC_LOADER_IGNORE_SRGB))
		{
			TinyComPtr<IWICMetadataQueryReader> metareader;
			if (SUCCEEDED(frame->GetMetadataQueryReader(&metareader)))
			{
				GUID containerFormat;
				if (SUCCEEDED(metareader->GetContainerFormat(&containerFormat)))
				{
					BOOL sRGB = FALSE;
					PROPVARIANT value;
					PropVariantInit(&value);
					if (memcmp(&containerFormat, &GUID_ContainerFormatPng, sizeof(GUID)) == 0)
					{
						if (SUCCEEDED(metareader->GetMetadataByName(L"/sRGB/RenderingIntent", &value)) && value.vt == VT_UI1)
						{
							sRGB = true;
						}
					}
					else if (SUCCEEDED(metareader->GetMetadataByName(L"System.Image.ColorSpace", &value)) && value.vt == VT_UI2 && value.uiVal == 1)
					{
						sRGB = true;
					}
					(void)PropVariantClear(&value);

					if (sRGB)
						format = MakeSRGB(format);
				}
			}
		}
		UINT support = 0;
		hRes = d3dDevice->CheckFormatSupport(format, &support);
		if (FAILED(hRes) || !(support & D3D11_FORMAT_SUPPORT_TEXTURE2D))
		{
			memcpy(&convertGUID, &GUID_WICPixelFormat32bppRGBA, sizeof(WICPixelFormatGUID));
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			bpp = 32;
		}
		size_t rowPitch = (twidth * bpp + 7) / 8;
		size_t imageSize = rowPitch * theight;

		std::unique_ptr<BYTE[]> temp(new (std::nothrow) BYTE[imageSize]);
		if (!temp)
			return E_OUTOFMEMORY;
		if (memcmp(&convertGUID, &pixelFormat, sizeof(GUID)) == 0
			&& twidth == width
			&& theight == height)
		{
			hRes = frame->CopyPixels(0, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), temp.get());
			if (hRes != S_OK)
				return hRes;
		}
		else if (twidth != width || theight != height)
		{
			auto pWIC = GetWIC();
			if (!pWIC)
				return E_NOINTERFACE;
			TinyComPtr<IWICBitmapScaler> scaler;
			hRes = pWIC->CreateBitmapScaler(&scaler);
			if (hRes != S_OK)
				return hRes;
			hRes = scaler->Initialize(frame, twidth, theight, WICBitmapInterpolationModeFant);
			if (hRes != S_OK)
				return hRes;
			WICPixelFormatGUID pfScaler;
			hRes = scaler->GetPixelFormat(&pfScaler);
			if (hRes != S_OK)
				return hRes;
			if (memcmp(&convertGUID, &pfScaler, sizeof(GUID)) == 0)
			{
				hRes = scaler->CopyPixels(0, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), temp.get());
				if (hRes != S_OK)
					return hRes;
			}
			else
			{
				TinyComPtr<IWICFormatConverter> FC;
				hRes = pWIC->CreateFormatConverter(&FC);
				if (hRes != S_OK)
					return hRes;
				BOOL canConvert = FALSE;
				hRes = FC->CanConvert(pfScaler, convertGUID, &canConvert);
				if (FAILED(hRes) || !canConvert)
				{
					return E_UNEXPECTED;
				}

				hRes = FC->Initialize(scaler, convertGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
				if (hRes != S_OK)
					return hRes;

				hRes = FC->CopyPixels(0, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), temp.get());
				if (hRes != S_OK)
					return hRes;
			}
		}
		else
		{
			auto pWIC = GetWIC();
			if (!pWIC)
				return E_NOINTERFACE;
			TinyComPtr<IWICFormatConverter> FC;
			hRes = pWIC->CreateFormatConverter(&FC);
			if (hRes != S_OK)
				return hRes;
			BOOL canConvert = FALSE;
			hRes = FC->CanConvert(pixelFormat, convertGUID, &canConvert);
			if (FAILED(hRes) || !canConvert)
			{
				return E_UNEXPECTED;
			}
			hRes = FC->Initialize(frame, convertGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
			if (hRes != S_OK)
				return hRes;
			hRes = FC->CopyPixels(0, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), temp.get());
			if (hRes != S_OK)
				return hRes;
		}
		BOOL autogen = FALSE;
		if (d3dContext != 0 && textureView != 0)
		{
			UINT fmtSupport = 0;
			hRes = d3dDevice->CheckFormatSupport(format, &fmtSupport);
			if (SUCCEEDED(hRes) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
			{
				autogen = TRUE;
			}
		}
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = twidth;
		desc.Height = theight;
		desc.MipLevels = (autogen) ? 0 : 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = usage;
		desc.CPUAccessFlags = cpuAccessFlags;

		if (autogen)
		{
			desc.BindFlags = bindFlags | D3D11_BIND_RENDER_TARGET;
			desc.MiscFlags = miscFlags | D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		else
		{
			desc.BindFlags = bindFlags;
			desc.MiscFlags = miscFlags;
		}

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = temp.get();
		initData.SysMemPitch = static_cast<UINT>(rowPitch);
		initData.SysMemSlicePitch = static_cast<UINT>(imageSize);
		ID3D11Texture2D* tex = nullptr;
		hRes = d3dDevice->CreateTexture2D(&desc, (autogen) ? nullptr : &initData, &tex);
		if (SUCCEEDED(hRes) && tex != 0)
		{
			if (textureView != 0)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
				SRVDesc.Format = desc.Format;
				SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				SRVDesc.Texture2D.MipLevels = (autogen) ? -1 : 1;
				hRes = d3dDevice->CreateShaderResourceView(tex, &SRVDesc, textureView);
				if (hRes != S_OK)
				{
					tex->Release();
					return hRes;
				}
				if (autogen)
				{
					ASSERT(d3dContext != 0);
					d3dContext->UpdateSubresource(tex, 0, nullptr, temp.get(), static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize));
					d3dContext->GenerateMips(*textureView);
				}
			}
			if (texture != 0)
			{
				*texture = tex;
			}
			else
			{
				tex->Release();
			}
		}
		return hRes;
	}

	HRESULT CreateWICTextureFromMemory(ID3D11Device* d3dDevice,
		const BYTE* wicData,
		size_t wicDataSize,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize)
	{
		return CreateWICTextureFromMemoryEx(d3dDevice, wicData, wicDataSize, maxsize,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT,
			texture, textureView);
	}


	HRESULT CreateWICTextureFromMemoryEx(ID3D11Device* d3dDevice,
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
		ID3D11ShaderResourceView** textureView)
	{
		if (texture)
		{
			*texture = nullptr;
		}
		if (textureView)
		{
			*textureView = nullptr;
		}

		if (!d3dDevice || !wicData || (!texture && !textureView))
			return E_INVALIDARG;

		if (!wicDataSize)
			return E_FAIL;

		if (wicDataSize > UINT32_MAX)
			return HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);

		auto pWIC = GetWIC();
		if (!pWIC)
			return E_NOINTERFACE;
		TinyComPtr<IWICStream> stream;
		HRESULT hRes = pWIC->CreateStream(&stream);
		if (hRes != S_OK)
			return hRes;
		hRes = stream->InitializeFromMemory(const_cast<BYTE*>(wicData), static_cast<DWORD>(wicDataSize));
		if (hRes != S_OK)
			return hRes;
		TinyComPtr<IWICBitmapDecoder> decoder;
		hRes = pWIC->CreateDecoderFromStream(stream, 0, WICDecodeMetadataCacheOnDemand, &decoder);
		if (hRes != S_OK)
			return hRes;
		TinyComPtr<IWICBitmapFrameDecode> frame;
		hRes = decoder->GetFrame(0, &frame);
		if (hRes != S_OK)
			return hRes;
		hRes = CreateTextureFromWIC(d3dDevice, d3dContext,
			frame, maxsize,
			usage, bindFlags, cpuAccessFlags, miscFlags, loadFlags,
			texture, textureView);
		if (hRes != S_OK)
			return hRes;
		return hRes;
	}

	HRESULT CreateWICTextureFromMemoryEx(ID3D11Device* d3dDevice,
		const BYTE* wicData,
		size_t wicDataSize,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView)
	{
		if (texture)
		{
			*texture = nullptr;
		}
		if (textureView)
		{
			*textureView = nullptr;
		}
		if (!d3dDevice || !wicData || (!texture && !textureView))
			return E_INVALIDARG;
		if (!wicDataSize)
			return E_FAIL;
		if (wicDataSize > UINT32_MAX)
			return HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);
		auto pWIC = GetWIC();
		if (!pWIC)
			return E_NOINTERFACE;

		TinyComPtr<IWICStream> stream;
		HRESULT hRes = pWIC->CreateStream(&stream);
		if (hRes != S_OK)
			return hRes;
		hRes = stream->InitializeFromMemory(const_cast<BYTE*>(wicData), static_cast<DWORD>(wicDataSize));
		if (hRes != S_OK)
			return hRes;
		TinyComPtr<IWICBitmapDecoder> decoder;
		hRes = pWIC->CreateDecoderFromStream(stream, 0, WICDecodeMetadataCacheOnDemand, &decoder);
		if (hRes != S_OK)
			return hRes;
		TinyComPtr<IWICBitmapFrameDecode> frame;
		hRes = decoder->GetFrame(0, &frame);
		if (hRes != S_OK)
			return hRes;
		hRes = CreateTextureFromWIC(d3dDevice, NULL,
			frame, maxsize,
			usage, bindFlags, cpuAccessFlags, miscFlags, loadFlags,
			texture, textureView);
		if (hRes != S_OK)
			return hRes;
		return hRes;
	}

	HRESULT CreateWICTextureFromMemory(ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		const BYTE* wicData,
		size_t wicDataSize,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize)
	{
		return CreateWICTextureFromMemoryEx(d3dDevice, d3dContext, wicData, wicDataSize, maxsize,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT,
			texture, textureView);
	}


	HRESULT CreateWICTextureFromFile(ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		const WCHAR* fileName,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize)
	{
		return CreateWICTextureFromFileEx(d3dDevice, d3dContext, fileName, maxsize,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT,
			texture, textureView);
	}

	HRESULT CreateWICTextureFromFile(ID3D11Device* d3dDevice,
		const WCHAR* fileName,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize)
	{
		return CreateWICTextureFromFileEx(d3dDevice, fileName, maxsize,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT,
			texture, textureView);
	}


	HRESULT CreateWICTextureFromFileEx(ID3D11Device* d3dDevice,
		const WCHAR* fileName,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView)
	{
		if (texture)
		{
			*texture = nullptr;
		}
		if (textureView)
		{
			*textureView = nullptr;
		}
		if (!d3dDevice || !fileName || (!texture && !textureView))
			return E_INVALIDARG;

		auto pWIC = GetWIC();
		if (!pWIC)
			return E_NOINTERFACE;
		TinyComPtr<IWICBitmapDecoder> decoder;
		HRESULT hRes = pWIC->CreateDecoderFromFilename(fileName, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
		if (hRes != S_OK)
			return hRes;
		TinyComPtr<IWICBitmapFrameDecode> frame;
		hRes = decoder->GetFrame(0, &frame);
		if (hRes != S_OK)
			return hRes;
		hRes = CreateTextureFromWIC(d3dDevice, NULL,
			frame, maxsize,
			usage, bindFlags, cpuAccessFlags, miscFlags, loadFlags,
			texture, textureView);
		return hRes;
	}

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
		ID3D11ShaderResourceView** textureView)
	{
		if (texture)
		{
			*texture = NULL;
		}
		if (textureView)
		{
			*textureView = NULL;
		}
		if (!d3dDevice || !szFileName || (!texture && !textureView))
		{
			return E_INVALIDARG;
		}
		auto pWIC = GetWIC();
		if (!pWIC)
			return E_NOINTERFACE;
		TinyComPtr<IWICBitmapDecoder> decoder;
		HRESULT hRes = pWIC->CreateDecoderFromFilename(szFileName, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
		if (hRes != S_OK)
			return hRes;
		TinyComPtr<IWICBitmapFrameDecode> frame;
		hRes = decoder->GetFrame(0, &frame);
		if (hRes != S_OK)
			return hRes;
		hRes = CreateTextureFromWIC(d3dDevice, NULL,
			frame.Ptr(), maxsize,
			usage, bindFlags, cpuAccessFlags, miscFlags, loadFlags,
			texture, textureView);
		return hRes;
	}

	inline DXGI_FORMAT EnsureNotTypeless(DXGI_FORMAT fmt)
	{
		switch (fmt)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case DXGI_FORMAT_R32G32B32_TYPELESS:    return DXGI_FORMAT_R32G32B32_FLOAT;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case DXGI_FORMAT_R32G32_TYPELESS:       return DXGI_FORMAT_R32G32_FLOAT;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:  return DXGI_FORMAT_R10G10B10A2_UNORM;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:     return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_R16G16_TYPELESS:       return DXGI_FORMAT_R16G16_UNORM;
		case DXGI_FORMAT_R32_TYPELESS:          return DXGI_FORMAT_R32_FLOAT;
		case DXGI_FORMAT_R8G8_TYPELESS:         return DXGI_FORMAT_R8G8_UNORM;
		case DXGI_FORMAT_R16_TYPELESS:          return DXGI_FORMAT_R16_UNORM;
		case DXGI_FORMAT_R8_TYPELESS:           return DXGI_FORMAT_R8_UNORM;
		case DXGI_FORMAT_BC1_TYPELESS:          return DXGI_FORMAT_BC1_UNORM;
		case DXGI_FORMAT_BC2_TYPELESS:          return DXGI_FORMAT_BC2_UNORM;
		case DXGI_FORMAT_BC3_TYPELESS:          return DXGI_FORMAT_BC3_UNORM;
		case DXGI_FORMAT_BC4_TYPELESS:          return DXGI_FORMAT_BC4_UNORM;
		case DXGI_FORMAT_BC5_TYPELESS:          return DXGI_FORMAT_BC5_UNORM;
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:     return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:     return DXGI_FORMAT_B8G8R8X8_UNORM;
		case DXGI_FORMAT_BC7_TYPELESS:          return DXGI_FORMAT_BC7_UNORM;
		default:                                return fmt;
		}
	}

	HRESULT CaptureTexture(ID3D11DeviceContext* pContext, ID3D11Resource* pSource, D3D11_TEXTURE2D_DESC& desc, TinyComPtr<ID3D11Texture2D>& pStaging)
	{
		if (!pContext || !pSource)
			return E_INVALIDARG;
		D3D11_RESOURCE_DIMENSION resType = D3D11_RESOURCE_DIMENSION_UNKNOWN;
		pSource->GetType(&resType);
		if (resType != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
			return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		TinyComPtr<ID3D11Texture2D> texture;
		HRESULT hRes = pSource->QueryInterface(&texture);
		if (hRes != S_OK)
			return hRes;
		ASSERT(texture);
		texture->GetDesc(&desc);
		TinyComPtr<ID3D11Device> d3dDevice;
		pContext->GetDevice(&d3dDevice);
		if (desc.SampleDesc.Count > 1)
		{
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			TinyComPtr<ID3D11Texture2D> newTexture;
			hRes = d3dDevice->CreateTexture2D(&desc, NULL, &newTexture);
			if (hRes != S_OK)
				return hRes;
			ASSERT(newTexture);
			DXGI_FORMAT fmt = EnsureNotTypeless(desc.Format);
			UINT support = 0;
			hRes = d3dDevice->CheckFormatSupport(fmt, &support);
			if (hRes != S_OK)
				return hRes;

			if (!(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE))
				return E_FAIL;

			for (UINT item = 0; item < desc.ArraySize; ++item)
			{
				for (UINT level = 0; level < desc.MipLevels; ++level)
				{
					UINT index = D3D11CalcSubresource(level, item, desc.MipLevels);
					pContext->ResolveSubresource(newTexture.Ptr(), index, pSource, index, fmt);
				}
			}
			desc.BindFlags = 0;
			desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.Usage = D3D11_USAGE_STAGING;
			pStaging.Release();
			hRes = d3dDevice->CreateTexture2D(&desc, 0, &pStaging);
			if (hRes != S_OK)
				return hRes;
			ASSERT(pStaging);
			pContext->CopyResource(pStaging, newTexture);
		}
		else if ((desc.Usage == D3D11_USAGE_STAGING) && (desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ))
		{
			pStaging = texture;
		}
		else
		{
			desc.BindFlags = 0;
			desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.Usage = D3D11_USAGE_STAGING;
			pStaging.Release();
			hRes = d3dDevice->CreateTexture2D(&desc, 0, &pStaging);
			if (hRes != S_OK)
				return hRes;
			ASSERT(pStaging);
			pContext->CopyResource(pStaging, pSource);
		}
		return S_OK;
	}

	HRESULT __cdecl SaveWICTextureToFile(
		ID3D11DeviceContext* pContext,
		ID3D11Resource* pSource,
		REFGUID guidContainerFormat,
		const WCHAR* pzName,
		const GUID* targetFormat,
		std::function<void __cdecl(IPropertyBag2*)> setCustomProps)
	{
		if (!pzName)
			return E_INVALIDARG;

		D3D11_TEXTURE2D_DESC desc = {};
		TinyComPtr<ID3D11Texture2D> pStaging;
		HRESULT hRes = CaptureTexture(pContext, pSource, desc, pStaging);
		if (hRes != S_OK)
			return hRes;
		WICPixelFormatGUID pfGuid;
		BOOL sRGB = FALSE;
		switch (desc.Format)
		{
		case DXGI_FORMAT_R32G32B32A32_FLOAT:            pfGuid = GUID_WICPixelFormat128bppRGBAFloat; break;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:            pfGuid = GUID_WICPixelFormat64bppRGBAHalf; break;
		case DXGI_FORMAT_R16G16B16A16_UNORM:            pfGuid = GUID_WICPixelFormat64bppRGBA; break;
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:    pfGuid = GUID_WICPixelFormat32bppRGBA1010102XR; break;
		case DXGI_FORMAT_R10G10B10A2_UNORM:             pfGuid = GUID_WICPixelFormat32bppRGBA1010102; break;
		case DXGI_FORMAT_B5G5R5A1_UNORM:                pfGuid = GUID_WICPixelFormat16bppBGRA5551; break;
		case DXGI_FORMAT_B5G6R5_UNORM:                  pfGuid = GUID_WICPixelFormat16bppBGR565; break;
		case DXGI_FORMAT_R32_FLOAT:                     pfGuid = GUID_WICPixelFormat32bppGrayFloat; break;
		case DXGI_FORMAT_R16_FLOAT:                     pfGuid = GUID_WICPixelFormat16bppGrayHalf; break;
		case DXGI_FORMAT_R16_UNORM:                     pfGuid = GUID_WICPixelFormat16bppGray; break;
		case DXGI_FORMAT_R8_UNORM:                      pfGuid = GUID_WICPixelFormat8bppGray; break;
		case DXGI_FORMAT_A8_UNORM:                      pfGuid = GUID_WICPixelFormat8bppAlpha; break;

		case DXGI_FORMAT_R8G8B8A8_UNORM:
			pfGuid = GUID_WICPixelFormat32bppRGBA;
			break;

		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			pfGuid = GUID_WICPixelFormat32bppRGBA;
			sRGB = TRUE;
			break;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
			pfGuid = GUID_WICPixelFormat32bppBGRA;
			break;

		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			pfGuid = GUID_WICPixelFormat32bppBGRA;
			sRGB = TRUE;
			break;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
			pfGuid = GUID_WICPixelFormat32bppBGR;
			break;

		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			pfGuid = GUID_WICPixelFormat32bppBGR;
			sRGB = TRUE;
			break;
		default:
			return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}
		auto pWIC = GetWIC();
		if (!pWIC)
			return E_NOINTERFACE;

		TinyComPtr<IWICStream> stream;
		hRes = pWIC->CreateStream(&stream);
		if (hRes != S_OK)
			return hRes;

		hRes = stream->InitializeFromFilename(pzName, GENERIC_WRITE);
		if (hRes != S_OK)
			return hRes;

		TinyComPtr<IWICBitmapEncoder> encoder;
		hRes = pWIC->CreateEncoder(guidContainerFormat, 0, &encoder);
		if (hRes != S_OK)
			return hRes;

		hRes = encoder->Initialize(stream, WICBitmapEncoderNoCache);
		if (hRes != S_OK)
			return hRes;

		TinyComPtr<IWICBitmapFrameEncode> frame;
		TinyComPtr<IPropertyBag2> props;
		hRes = encoder->CreateNewFrame(&frame, &props);
		if (hRes != S_OK)
			return hRes;
		if (targetFormat && memcmp(&guidContainerFormat, &GUID_ContainerFormatBmp, sizeof(WICPixelFormatGUID)) == 0 && IsWIC2())
		{
			PROPBAG2 option = {};
			option.pstrName = L"EnableV5Header32bppBGRA";
			VARIANT varValue;
			varValue.vt = VT_BOOL;
			varValue.boolVal = VARIANT_TRUE;
			(void)props->Write(1, &option, &varValue);
		}
		if (setCustomProps)
		{
			setCustomProps(props);
		}
		hRes = frame->Initialize(props);
		if (hRes != S_OK)
			return hRes;
		hRes = frame->SetSize(desc.Width, desc.Height);
		if (hRes != S_OK)
			return hRes;
		hRes = frame->SetResolution(72, 72);
		if (hRes != S_OK)
			return hRes;
		WICPixelFormatGUID targetGuid;
		if (targetFormat)
		{
			targetGuid = *targetFormat;
		}
		else
		{
			switch (desc.Format)
			{
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
				if (IsWIC2())
				{
					targetGuid = GUID_WICPixelFormat96bppRGBFloat;
				}
				else
				{
					targetGuid = GUID_WICPixelFormat24bppBGR;
				}
				break;
#endif

			case DXGI_FORMAT_R16G16B16A16_UNORM: targetGuid = GUID_WICPixelFormat48bppBGR; break;
			case DXGI_FORMAT_B5G5R5A1_UNORM:     targetGuid = GUID_WICPixelFormat16bppBGR555; break;
			case DXGI_FORMAT_B5G6R5_UNORM:       targetGuid = GUID_WICPixelFormat16bppBGR565; break;

			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_R16_UNORM:
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_A8_UNORM:
				targetGuid = GUID_WICPixelFormat8bppGray;
				break;

			default:
				targetGuid = GUID_WICPixelFormat24bppBGR;
				break;
			}
		}

		hRes = frame->SetPixelFormat(&targetGuid);
		if (hRes != S_OK)
			return hRes;

		if (targetFormat && memcmp(targetFormat, &targetGuid, sizeof(WICPixelFormatGUID)) != 0)
		{
			return E_FAIL;
		}
		TinyComPtr<IWICMetadataQueryWriter> metawriter;
		if (SUCCEEDED(frame->GetMetadataQueryWriter(&metawriter)))
		{
			PROPVARIANT value;
			PropVariantInit(&value);
			value.vt = VT_LPSTR;
			value.pszVal = "WIC";
			if (memcmp(&guidContainerFormat, &GUID_ContainerFormatPng, sizeof(GUID)) == 0)
			{
				(void)metawriter->SetMetadataByName(L"/tEXt/{str=Software}", &value);
				if (sRGB)
				{
					value.vt = VT_UI1;
					value.bVal = 0;
					(void)metawriter->SetMetadataByName(L"/sRGB/RenderingIntent", &value);
				}
			}
			(void)metawriter->SetMetadataByName(L"System.ApplicationName", &value);
			if (sRGB)
			{
				value.vt = VT_UI2;
				value.uiVal = 1;
				(void)metawriter->SetMetadataByName(L"System.Image.ColorSpace", &value);
			}
		}

		D3D11_MAPPED_SUBRESOURCE mapped;
		hRes = pContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped);
		if (hRes != S_OK)
			return hRes;
		if (memcmp(&targetGuid, &pfGuid, sizeof(WICPixelFormatGUID)) != 0)
		{
			TinyComPtr<IWICBitmap> source;
			hRes = pWIC->CreateBitmapFromMemory(desc.Width, desc.Height, pfGuid, mapped.RowPitch, mapped.RowPitch * desc.Height, reinterpret_cast<BYTE*>(mapped.pData), &source);
			if (hRes != S_OK)
			{
				pContext->Unmap(pStaging, 0);
				return hRes;
			}
			TinyComPtr<IWICFormatConverter> FC;
			hRes = pWIC->CreateFormatConverter(&FC);
			if (hRes != S_OK)
			{
				pContext->Unmap(pStaging, 0);
				return hRes;
			}
			BOOL canConvert = FALSE;
			hRes = FC->CanConvert(pfGuid, targetGuid, &canConvert);
			if (FAILED(hRes) || !canConvert)
			{
				return E_UNEXPECTED;
			}
			hRes = FC->Initialize(source, targetGuid, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeCustom);
			if (hRes != S_OK)
			{
				pContext->Unmap(pStaging, 0);
				return hRes;
			}
			WICRect rect = { 0, 0, static_cast<INT>(desc.Width), static_cast<INT>(desc.Height) };
			hRes = frame->WriteSource(FC, &rect);
			if (hRes != S_OK)
			{
				pContext->Unmap(pStaging, 0);
				return hRes;
			}
		}
		else
		{
			hRes = frame->WritePixels(desc.Height, mapped.RowPitch, mapped.RowPitch * desc.Height, reinterpret_cast<BYTE*>(mapped.pData));
			if (hRes != S_OK)
				return hRes;
		}
		pContext->Unmap(pStaging, 0);
		hRes = frame->Commit();
		if (hRes != S_OK)
			return hRes;
		hRes = encoder->Commit();
		if (hRes != S_OK)
			return hRes;
		return S_OK;
	}

}