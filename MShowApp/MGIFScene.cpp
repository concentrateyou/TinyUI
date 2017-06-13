#include "stdafx.h"
#include "MGIFScene.h"
#include "MShowController.h"

namespace MShow
{
	const FLOAT DEFAULT_DPI = 96.F;
	const UINT DELAY_TIMER_ID = 1;

	IMPLEMENT_DYNAMIC(MGIFScene, MElement);

	MGIFScene::MGIFScene(MShowController* pController)
		:m_pController(pController),
		m_cxGifImage(0),
		m_cyGifImage(0),
		m_cxGifImagePixel(0),
		m_cyGifImagePixel(0),
		m_delay(0),
		m_count(0),
		m_index(0),
		m_hTimer(NULL)
	{
	}

	MGIFScene::~MGIFScene()
	{
		Close();
	}

	BOOL MGIFScene::Initialize(DX2D& d2d, const CHAR* pzFile)
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
		if (hRes != S_OK)
			return FALSE;
		hRes = GetGlobalMetadata(ps);
		if (hRes != S_OK)
			return FALSE;
		D2D1_SIZE_F sizeF = { static_cast<FLOAT>(m_cxGifImage), static_cast<FLOAT>(m_cyGifImage) };
		hRes = d2d.GetContext()->CreateCompatibleRenderTarget(sizeF, &m_bitmapRT);
		if (hRes != S_OK)
			return FALSE;
		for (UINT i = 0;i < m_count;i++)
		{
			hRes |= GetFrame(d2d, ps, i);
		}
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<ID2D1Bitmap> bitmap;
		hRes = m_bitmapRT->GetBitmap(&bitmap);
		if (hRes != S_OK)
			return FALSE;
		auto bitmapSize = bitmap->GetPixelSize();
		D2D1_BITMAP_PROPERTIES bitmapProp;
		bitmap->GetDpi(&bitmapProp.dpiX, &bitmapProp.dpiY);
		bitmapProp.pixelFormat = bitmap->GetPixelFormat();
		hRes = m_bitmapRT->CreateBitmap(
			bitmapSize,
			bitmapProp,
			&m_bitmap);
		TinySize size(m_cxGifImage, m_cyGifImage);
		SetSize(size);
		SetScale(size);
		return SUCCEEDED(hRes);
	}

	BOOL MGIFScene::Submit()
	{
		ASSERT(m_pController);
		m_hTimer = m_pController->GetTimerQueue()->Register(&MGIFScene::Callback, this, m_images[m_index].delay, 1);
		return m_hTimer != NULL;
	}

	VOID CALLBACK MGIFScene::Callback(PVOID lpParameter, BOOLEAN val)
	{
		MGIFScene* ps = reinterpret_cast<MGIFScene*>(lpParameter);
		if (ps && val)
		{
			ps->m_pController->GetTimerQueue()->Change(ps->m_hTimer, ps->m_images[ps->m_index].delay, 1);
			ps->m_pController->Draw(ps);
			ps->m_index = (++ps->m_index) % ps->m_count;
		}
	}

	BOOL MGIFScene::Close()
	{
		m_pController->GetTimerQueue()->Unregister(m_hTimer);
		for (UINT i = 0;i < m_images.GetSize();i++)
		{
			SAFE_RELEASE(m_images[i].bitmap);
		}
		m_images.RemoveAll();
		return TRUE;
	}

	BOOL MGIFScene::Draw(DX2D& d2d)
	{
		ASSERT(m_pController);
		HRESULT hRes = DrawFrame(d2d, m_index);
		return SUCCEEDED(hRes);
	}

	HRESULT MGIFScene::ClearFrame(UINT index)
	{
		m_bitmapRT->BeginDraw();
		m_bitmapRT->PushAxisAlignedClip(&m_images[index].rectF, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		m_bitmapRT->Clear(m_backgroundColor);
		m_bitmapRT->PopAxisAlignedClip();
		return m_bitmapRT->EndDraw();
	}

	HRESULT MGIFScene::SaveComposedFrame()
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID2D1Bitmap> bitmap;
		hRes = m_bitmapRT->GetBitmap(&bitmap);
		if (SUCCEEDED(hRes))
		{
			if (m_bitmap == NULL)
			{
				auto bitmapSize = bitmap->GetPixelSize();
				D2D1_BITMAP_PROPERTIES bitmapProp;
				bitmap->GetDpi(&bitmapProp.dpiX, &bitmapProp.dpiY);
				bitmapProp.pixelFormat = bitmap->GetPixelFormat();
				hRes = m_bitmapRT->CreateBitmap(
					bitmapSize,
					bitmapProp,
					&m_bitmap);
			}
		}
		if (SUCCEEDED(hRes))
		{
			hRes = m_bitmap->CopyFromBitmap(nullptr, bitmap, nullptr);
		}
		return hRes;
	}

	HRESULT MGIFScene::RestoreSavedFrame()
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID2D1Bitmap> bitmap;
		hRes = m_bitmap ? S_OK : E_FAIL;
		if (SUCCEEDED(hRes))
		{
			hRes = m_bitmapRT->GetBitmap(&bitmap);
		}
		if (SUCCEEDED(hRes))
		{
			hRes = bitmap->CopyFromBitmap(nullptr, m_bitmap, nullptr);
		}
		return hRes;
	}

	HRESULT MGIFScene::DisposeFrame(UINT index)
	{
		HRESULT hRes = S_OK;
		WIC_GIF& gif = m_images[index];
		switch (gif.disposal)
		{
		case DM_UNDEFINED:
		case DM_NONE:
			break;
		case DM_BACKGROUND:
			hRes = ClearFrame(index);
			break;
		case DM_PREVIOUS:
			hRes = RestoreSavedFrame();
			break;
		default:
			hRes = E_FAIL;
		}
		return hRes;
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

	HRESULT MGIFScene::DrawFrame(DX2D& d2d, UINT index)
	{
		HRESULT hRes = DisposeFrame(index);
		if (SUCCEEDED(hRes))
		{
			if (m_images[index].disposal == DM_PREVIOUS)
			{
				hRes = SaveComposedFrame();
			}
		}
		if (SUCCEEDED(hRes))
		{
			m_bitmapRT->BeginDraw();
			if (m_index == 0)
			{
				m_bitmapRT->Clear(m_backgroundColor);
			}
			m_bitmapRT->DrawBitmap(m_images[index].bitmap, m_images[index].rectF);
			hRes = m_bitmapRT->EndDraw();
		}
		if (SUCCEEDED(hRes))
		{
			TinyPoint pos = GetPosition();
			TinySize size = GetScale();
			D2D_RECT_F dst = { static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y),static_cast<FLOAT>(pos.x + size.cx),static_cast<FLOAT>(pos.y + size.cy) };
			D2D_RECT_F src = { 0.0F,0.0F,static_cast<LONG>(m_size.cx), static_cast<LONG>(m_size.cy) };
			TinyComPtr<ID2D1Bitmap> bitmap;
			m_bitmapRT->GetBitmap(&bitmap);
			d2d.GetContext()->DrawBitmap(bitmap, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
		}
		return hRes;
	}

	HRESULT MGIFScene::GetFrame(DX2D& d2d, IWICImagingFactory* ps, UINT index)
	{
		TinyComPtr<IWICFormatConverter> converter;
		TinyComPtr<IWICBitmapFrameDecode> decode;
		TinyComPtr<IWICMetadataQueryReader> metadataQueryReader;
		PROPVARIANT propValue;
		PropVariantInit(&propValue);
		HRESULT hRes = m_decoder->GetFrame(index, &decode);
		if (SUCCEEDED(hRes))
		{
			hRes = ps->CreateFormatConverter(&converter);
		}
		if (SUCCEEDED(hRes))
		{
			hRes = converter->Initialize(
				decode,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				nullptr,
				0.f,
				WICBitmapPaletteTypeCustom);
		}
		WIC_GIF gif = { 0 };
		if (SUCCEEDED(hRes))
		{
			hRes = d2d.GetContext()->CreateBitmapFromWicBitmap(
				converter,
				nullptr,
				&gif.bitmap);
		}
		if (SUCCEEDED(hRes))
		{
			hRes = decode->GetMetadataQueryReader(&metadataQueryReader);
		}
		if (SUCCEEDED(hRes))
		{
			hRes = metadataQueryReader->GetMetadataByName(L"/imgdesc/Left", &propValue);
			if (SUCCEEDED(hRes))
			{
				hRes = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
				if (SUCCEEDED(hRes))
				{
					gif.rectF.left = static_cast<float>(propValue.uiVal);
				}
				PropVariantClear(&propValue);
			}
		}
		if (SUCCEEDED(hRes))
		{
			hRes = metadataQueryReader->GetMetadataByName(L"/imgdesc/Top", &propValue);
			if (SUCCEEDED(hRes))
			{
				hRes = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
				if (SUCCEEDED(hRes))
				{
					gif.rectF.top = static_cast<float>(propValue.uiVal);
				}
				PropVariantClear(&propValue);
			}
		}
		if (SUCCEEDED(hRes))
		{
			hRes = metadataQueryReader->GetMetadataByName(L"/imgdesc/Width", &propValue);
			if (SUCCEEDED(hRes))
			{
				hRes = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
				if (SUCCEEDED(hRes))
				{
					gif.rectF.right = static_cast<float>(propValue.uiVal) + gif.rectF.left;
				}
				PropVariantClear(&propValue);
			}
		}
		if (SUCCEEDED(hRes))
		{
			hRes = metadataQueryReader->GetMetadataByName(L"/imgdesc/Height", &propValue);
			if (SUCCEEDED(hRes))
			{
				hRes = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
				if (SUCCEEDED(hRes))
				{
					gif.rectF.bottom = static_cast<float>(propValue.uiVal) + gif.rectF.top;
				}
				PropVariantClear(&propValue);
			}
		}
		if (SUCCEEDED(hRes))
		{
			if (SUCCEEDED(metadataQueryReader->GetMetadataByName(
				L"/grctlext/Delay",
				&propValue)))
			{
				hRes = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
				if (SUCCEEDED(hRes))
				{
					hRes = UIntMult(propValue.uiVal, 10, &gif.delay);
				}
				PropVariantClear(&propValue);
			}
			else
			{
				gif.delay = 0;
			}
		}
		if (SUCCEEDED(hRes))
		{
			if (SUCCEEDED(metadataQueryReader->GetMetadataByName(
				L"/grctlext/Disposal",
				&propValue)))
			{
				hRes = (propValue.vt == VT_UI1) ? S_OK : E_FAIL;
				if (SUCCEEDED(hRes))
				{
					gif.disposal = propValue.bVal;
				}
			}
			else
			{
				gif.disposal = DM_UNDEFINED;
			}
		}
		PropVariantClear(&propValue);
		if (SUCCEEDED(hRes))
		{
			m_images.Add(gif);
		}
		return hRes;
	}
}
