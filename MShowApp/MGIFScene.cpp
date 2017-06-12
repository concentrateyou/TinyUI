#include "stdafx.h"
#include "MGIFScene.h"


namespace MShow
{
	const FLOAT DEFAULT_DPI = 96.F;

	IMPLEMENT_DYNAMIC(MGIFScene, MElement);

	MGIFScene::MGIFScene()
		:m_cxGifImage(0),
		m_cyGifImage(0),
		m_cxGifImagePixel(0),
		m_cyGifImagePixel(0),
		m_loops(0),
		m_count(0)
	{
	}

	MGIFScene::~MGIFScene()
	{
	}
	BOOL MGIFScene::Load(DX2D& d2d, const CHAR* pzFile)
	{
		ASSERT(d2d.GetContext());
		auto ps = GetWIC();
		if (!ps)
			return FALSE;
		wstring ws = StringToWString(pzFile);
		HRESULT hRes = ps->CreateDecoderFromFilename(
			ws.c_str(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&m_decoder);
		if (SUCCEEDED(hRes))
		{
			hRes = GetGlobalMetadata(ps);
		}
		if (SUCCEEDED(hRes))
		{
			D2D1_SIZE_F sizeF = { static_cast<FLOAT>(m_cxGifImage), static_cast<FLOAT>(m_cyGifImage) };
			hRes = d2d.GetContext()->CreateCompatibleRenderTarget(sizeF, &m_bitmapRT);
		}
		return SUCCEEDED(hRes);
	}

	HRESULT MGIFScene::GetGlobalMetadata(IWICImagingFactory* ps)
	{
		PROPVARIANT propValue;
		PropVariantInit(&propValue);
		TinyComPtr<IWICMetadataQueryReader> metadataQueryReader;
		HRESULT hRes = m_decoder->GetFrameCount(&m_count);
		if (SUCCEEDED(hRes))
		{
			hRes = m_decoder->GetMetadataQueryReader(&metadataQueryReader);
		}
		if (SUCCEEDED(hRes))
		{
			if (FAILED(GetBackgroundColor(ps, metadataQueryReader)))
			{
				m_backgroundColor = D2D1::ColorF(0, 0.f);
			}
		}

		if (SUCCEEDED(hRes))
		{
			hRes = metadataQueryReader->GetMetadataByName(L"/logscrdesc/Width", &propValue);
			if (SUCCEEDED(hRes))
			{
				hRes = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
				if (SUCCEEDED(hRes))
				{
					m_cxGifImage = propValue.uiVal;
				}
				PropVariantClear(&propValue);
			}
		}

		if (SUCCEEDED(hRes))
		{
			hRes = metadataQueryReader->GetMetadataByName(L"/logscrdesc/Height", &propValue);
			if (SUCCEEDED(hRes))
			{
				hRes = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
				if (SUCCEEDED(hRes))
				{
					m_cyGifImage = propValue.uiVal;
				}
				PropVariantClear(&propValue);
			}
		}
		if (SUCCEEDED(hRes))
		{
			hRes = metadataQueryReader->GetMetadataByName(L"/logscrdesc/PixelAspectRatio", &propValue);
			if (SUCCEEDED(hRes))
			{
				hRes = (propValue.vt == VT_UI1 ? S_OK : E_FAIL);
				if (SUCCEEDED(hRes))
				{
					UINT uPixelAspRatio = propValue.bVal;
					if (uPixelAspRatio != 0)
					{
						FLOAT pixelAspRatio = (uPixelAspRatio + 15.F) / 64.F;
						if (pixelAspRatio > 1.F)
						{
							m_cxGifImagePixel = m_cxGifImage;
							m_cyGifImagePixel = static_cast<UINT>(m_cyGifImage / pixelAspRatio);
						}
						else
						{
							m_cxGifImagePixel = static_cast<UINT>(m_cxGifImage * pixelAspRatio);
							m_cyGifImagePixel = m_cyGifImage;
						}
					}
					else
					{
						m_cxGifImagePixel = m_cxGifImage;
						m_cyGifImagePixel = m_cyGifImage;
					}
				}
				PropVariantClear(&propValue);
			}
		}
		if (SUCCEEDED(hRes))
		{
			if (SUCCEEDED(metadataQueryReader->GetMetadataByName(
				L"/appext/application",
				&propValue)) &&
				propValue.vt == (VT_UI1 | VT_VECTOR) &&
				propValue.caub.cElems == 11 &&
				(!memcmp(propValue.caub.pElems, "NETSCAPE2.0", propValue.caub.cElems) ||
					!memcmp(propValue.caub.pElems, "ANIMEXTS1.0", propValue.caub.cElems)))
			{
				PropVariantClear(&propValue);
				hRes = metadataQueryReader->GetMetadataByName(L"/appext/data", &propValue);
				if (SUCCEEDED(hRes))
				{
					if (propValue.vt == (VT_UI1 | VT_VECTOR) &&
						propValue.caub.cElems >= 4 &&
						propValue.caub.pElems[0] > 0 &&
						propValue.caub.pElems[1] == 1)
					{
						m_loops = MAKEWORD(propValue.caub.pElems[2], propValue.caub.pElems[3]);
					}
				}
			}
		}
		PropVariantClear(&propValue);
		return hRes;
	}

	HRESULT MGIFScene::GetBackgroundColor(IWICImagingFactory* ps, IWICMetadataQueryReader *pMetadataQueryReader)
	{
		DWORD dwBGColor;
		BYTE backgroundIndex = 0;
		WICColor rgColors[256];
		UINT cColorsCopied = 0;
		PROPVARIANT propVariant;
		PropVariantInit(&propVariant);
		TinyComPtr<IWICPalette> palette;
		HRESULT hRes = pMetadataQueryReader->GetMetadataByName(
			L"/logscrdesc/GlobalColorTableFlag",
			&propVariant);
		if (SUCCEEDED(hRes))
		{
			hRes = (propVariant.vt != VT_BOOL || !propVariant.boolVal) ? E_FAIL : S_OK;
			PropVariantClear(&propVariant);
		}
		if (SUCCEEDED(hRes))
		{
			hRes = pMetadataQueryReader->GetMetadataByName(
				L"/logscrdesc/BackgroundColorIndex",
				&propVariant);
			if (SUCCEEDED(hRes))
			{
				hRes = (propVariant.vt != VT_UI1) ? E_FAIL : S_OK;
				if (SUCCEEDED(hRes))
				{
					backgroundIndex = propVariant.bVal;
				}
				PropVariantClear(&propVariant);
			}
		}
		if (SUCCEEDED(hRes))
		{
			hRes = ps->CreatePalette(&palette);
		}

		if (SUCCEEDED(hRes))
		{
			hRes = m_decoder->CopyPalette(palette);
		}

		if (SUCCEEDED(hRes))
		{
			hRes = palette->GetColors(
				ARRAYSIZE(rgColors),
				rgColors,
				&cColorsCopied);
		}

		if (SUCCEEDED(hRes))
		{
			hRes = (backgroundIndex >= cColorsCopied) ? E_FAIL : S_OK;
		}

		if (SUCCEEDED(hRes))
		{
			dwBGColor = rgColors[backgroundIndex];
			FLOAT alpha = (dwBGColor >> 24) / 255.F;
			m_backgroundColor = D2D1::ColorF(dwBGColor, alpha);
		}
		return hRes;
	}
}
