#pragma once
#include "MPreviewController.h"

namespace MShow
{
	class MPreviewController;

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
	/// Í¼Æ¬³¡¾°
	/// </summary>
	class MImageModel : public MElement
	{
		DISALLOW_COPY_AND_ASSIGN(MImageModel)
	public:
		MImageModel(MPreviewController& controller, Callback<void(ID2D1Bitmap1*)>&& callback);
		virtual ~MImageModel();
		BOOL Initialize(DX2D& dx2d, const CHAR* pzFile);
		BOOL Animate();
	public:
		BOOL Draw(DX2D& d2d) OVERRIDE;
		BOOL Release() OVERRIDE;
	public:
		ID2D1Bitmap1**	GetBitmap();
	private:
		static VOID CALLBACK TimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
		HRESULT DrawFrame(DX2D& d2d, UINT index);
		HRESULT GetFrame(DX2D& d2d, IWICImagingFactory* ps, UINT index);
		HRESULT GetGlobalMetadata(IWICImagingFactory* ps);
		HRESULT GetBackgroundColor(IWICImagingFactory* ps, IWICMetadataQueryReader *pMetadataQueryReader);
		HRESULT ClearFrame(UINT index);
		HRESULT DisposeFrame(UINT index);
		HRESULT SaveComposedFrame();
		HRESULT	RestoreSavedFrame();
	private:
		GUID								m_guidCF;
		UINT								m_delay;
		UINT								m_index;
		UINT								m_count;
		UINT								m_cxGifImage;
		UINT								m_cyGifImage;
		UINT								m_cxGifImagePixel;
		UINT								m_cyGifImagePixel;
		HANDLE								m_hTimer;
		D2D1_COLOR_F						m_backgroundColor;
		TinyArray<WIC_GIF>					m_images;
		Callback<void(ID2D1Bitmap1*)>		m_callback;
		MPreviewController&					m_controller;
		TinyComPtr<ID2D1Bitmap>				m_bitmap;
		TinyComPtr<ID2D1Bitmap1>			m_bitmap1;
		TinyComPtr<IWICBitmapDecoder>		m_decoder;
		TinyComPtr<ID2D1BitmapRenderTarget>	m_bitmapRT;
	};
}


