#pragma once
#include "MElement.h"
#include "WICTexture.h"

namespace MShow
{
	typedef struct tagWIC_GIF
	{
		UINT			delay;
		UINT			disposal;
		D2D_RECT_F		rectF;
		ID2D1Bitmap1*	bitmap;
	}WIC_GIF;

	enum DISPOSAL_METHODS
	{
		DM_UNDEFINED = 0,
		DM_NONE = 1,
		DM_BACKGROUND = 2,
		DM_PREVIOUS = 3
	};
	class MPreviewController;
	/// <summary>
	/// GIF³¡¾°
	/// </summary>
	class MGIFScene : public MElement
	{
		DECLARE_DYNAMIC(MGIFScene)
	public:
		MGIFScene(MPreviewController* pController);
		virtual ~MGIFScene();
	public:
		BOOL Initialize(DX2D& d2d, const CHAR* pzFile);
	public:
		BOOL Draw(DX2D& d2d) OVERRIDE;
		BOOL Submit() OVERRIDE;
		BOOL Close() OVERRIDE;
	private:
		static VOID CALLBACK Callback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
		HRESULT DrawFrame(DX2D& d2d, UINT index);
		HRESULT GetFrame(DX2D& d2d, IWICImagingFactory* ps, UINT index);
		HRESULT GetGlobalMetadata(IWICImagingFactory* ps);
		HRESULT GetBackgroundColor(IWICImagingFactory* ps, IWICMetadataQueryReader *pMetadataQueryReader);
		HRESULT ClearFrame(UINT index);
		HRESULT DisposeFrame(UINT index);
		HRESULT SaveComposedFrame();
		HRESULT	RestoreSavedFrame();
	private:
		UINT								m_delay;
		UINT								m_index;
		UINT								m_count;
		UINT								m_cxGifImage;
		UINT								m_cyGifImage;
		UINT								m_cxGifImagePixel;
		UINT								m_cyGifImagePixel;
		HANDLE								m_hTimer;
		D2D1_COLOR_F						m_backgroundColor;
		TinyComPtr<ID2D1Bitmap>				m_bitmap;//±£´æ
		TinyComPtr<IWICBitmapDecoder>		m_decoder;
		TinyComPtr<ID2D1BitmapRenderTarget>	m_bitmapRT;
		TinyArray<WIC_GIF>					m_images;
		MPreviewController*					m_pController;
	};
}


