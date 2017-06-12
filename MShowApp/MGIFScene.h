#pragma once
#include "MElement.h"
#include "WICTexture.h"

namespace MShow
{
	/// <summary>
	/// GIF³¡¾°
	/// </summary>
	class MGIFScene : public MElement
	{
		DECLARE_DYNAMIC(MGIFScene)
	public:
		MGIFScene();
		virtual ~MGIFScene();
	public:
		BOOL Load(DX2D& d2d, const CHAR* pzFile);
	private:
		HRESULT GetGlobalMetadata(IWICImagingFactory* ps);
		HRESULT GetBackgroundColor(IWICImagingFactory* ps,IWICMetadataQueryReader *pMetadataQueryReader);;
	private:						
		UINT								m_count;
		UINT								m_loops;
		UINT								m_cxGifImage;
		UINT								m_cyGifImage;
		UINT								m_cxGifImagePixel;
		UINT								m_cyGifImagePixel;
		D2D1_COLOR_F						m_backgroundColor;
		TinyComPtr<ID2D1Bitmap1>			m_bitmap;
		TinyComPtr<IWICBitmapDecoder>		m_decoder;
		TinyComPtr<ID2D1BitmapRenderTarget>	m_bitmapRT;
	};
}


