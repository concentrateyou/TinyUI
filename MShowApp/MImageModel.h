#pragma once
#include "MPreviewController.h"
#include "Render/TinyImage.h"

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
		MImageModel(MPreviewController& controller, DWORD dwIndex, Callback<void(BYTE*, LONG)>&& callback);
		virtual ~MImageModel();
		BOOL	 Initialize(const CHAR* pzFile);
		BOOL	 Animate();
	public:
		BOOL Draw(FLOAT radioX, FLOAT radioY) OVERRIDE;
		BOOL Release() OVERRIDE;
	public:
		ID2D1Bitmap1*	GetBitmap();
	private:
		static VOID CALLBACK TimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
	private:
		UINT								m_index;
		HANDLE								m_hTimer;
		TinyImage							m_image;
		TinyComPtr<ID2D1Bitmap1>			m_bitmap1;
		MPreviewController&					m_controller;
		Callback<void(BYTE*, LONG)>			m_callback;
	};
}


