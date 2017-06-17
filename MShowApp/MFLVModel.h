#pragma once
#include "MPreviewController.h"

namespace MShow
{
	/// <summary>
	/// FLV³¡¾°
	/// </summary>
	class MFLVModel : public MElement
	{
		DISALLOW_COPY_AND_ASSIGN(MFLVModel)
	public:
		MFLVModel(MPreviewController* pController);
		virtual ~MFLVModel();
		BOOL Draw(DX2D& d2d) OVERRIDE;
		BOOL Initialize(LPVOID ps) OVERRIDE;
		BOOL Uninitialize(LPVOID ps) OVERRIDE;
	public:
		ID2D1Bitmap1*	GetBitmap();
	private:
		MPreviewController*			m_pController;
		TinyComPtr<ID2D1Bitmap1>	m_bitmap;
	};
}


