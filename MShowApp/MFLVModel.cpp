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

	BOOL MFLVModel::Initialize(const TinySize& size)
	{
		HRESULT hRes = m_controller.GetD2D().GetContext()->CreateBitmap(D2D1::SizeU(size.cx, size.cy),
			(const void *)NULL,
			0,
			&D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			&m_bitmap1);
		return SUCCEEDED(hRes);
	}

	ID2D1Bitmap1* MFLVModel::GetBitmap()
	{
		return m_bitmap1;
	}

	BOOL MFLVModel::Draw()
	{
		if (m_bitmap1 != NULL)
		{
			TinyPoint pos = GetPosition();
			TinySize size = GetScale();
			D2D_RECT_F dst = { static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y),static_cast<FLOAT>(pos.x + size.cx),static_cast<FLOAT>(pos.y + size.cy) };
			D2D_RECT_F src = { 0.0F,0.0F,static_cast<FLOAT>(m_size.cx), static_cast<FLOAT>(m_size.cy) };
			m_controller.GetD2D().GetContext()->DrawBitmap(m_bitmap1, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
			return TRUE;
		}
		return FALSE;
	}

	BOOL MFLVModel::Release()
	{
		m_bitmap1.Release();
		return TRUE;
	}
}