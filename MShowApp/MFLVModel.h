#pragma once
#include "MPreviewController.h"
#include "ImageView.h"

namespace MShow
{
	class MPreviewController;
	/// <summary>
	/// FLV³¡¾°
	/// </summary>
	class MFLVModel : public MElement
	{
		DISALLOW_COPY_AND_ASSIGN(MFLVModel)
	public:
		MFLVModel(MPreviewController& controller);
		virtual ~MFLVModel();
		BOOL	Initialize(const TinySize& size);
	public:
		BOOL	Draw(FLOAT radioX, FLOAT radioY) OVERRIDE;
		BOOL	Release() OVERRIDE;
	public:
		ID2D1Bitmap1*	GetBitmap();
	private:
		MPreviewController&			m_controller;
		TinyComPtr<ID2D1Bitmap1>	m_bitmap1;
	};
}


