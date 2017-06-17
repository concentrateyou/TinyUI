#include "stdafx.h"
#include "MFLVModel.h"

namespace MShow
{
	MFLVModel::MFLVModel(MPreviewController& controller)
		:m_controller(controller)
	{
	}


	MFLVModel::~MFLVModel()
	{
		Release();
	}

	ID2D1Bitmap1** MFLVModel::GetBitmap()
	{
		return &m_bitmap;
	}

	BOOL MFLVModel::Draw(DX2D& d2d)
	{
		if (m_bitmap != NULL)
		{
			TinyPoint pos = GetPosition();
			TinySize size = GetScale();
			D2D_RECT_F dst = { static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y),static_cast<FLOAT>(pos.x + size.cx),static_cast<FLOAT>(pos.y + size.cy) };
			D2D_RECT_F src = { 0.0F,0.0F,static_cast<FLOAT>(m_size.cx), static_cast<FLOAT>(m_size.cy) };
			d2d.GetContext()->DrawBitmap(m_bitmap, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
			return TRUE;
		}
		return FALSE;
	}

	BOOL MFLVModel::Release()
	{
		m_bitmap.Release();
		return TRUE;
	}
}