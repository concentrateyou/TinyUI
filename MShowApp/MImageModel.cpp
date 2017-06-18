#include "stdafx.h"
#include "MImageModel.h"
#include "WICTexture.h"

namespace MShow
{
	MImageModel::MImageModel(MPreviewController& controller, Callback<void(BYTE*, LONG)>&& callback)
		:m_controller(controller),
		m_callback(std::move(callback)),
		m_hTimer(NULL),
		m_index(0)
	{
	}


	MImageModel::~MImageModel()
	{
		Release();
	}

	VOID CALLBACK MImageModel::TimerCallback(PVOID lpParameter, BOOLEAN val)
	{
		MImageModel* pThis = reinterpret_cast<MImageModel*>(lpParameter);
		if (pThis && val)
		{
			INT count = pThis->m_image.GetCount();
			if (count > 0)
			{
				TinyApplication::GetInstance()->GetTimers().Change(pThis->m_hTimer, pThis->m_image.GetDelay(pThis->m_index), 1);
				if (!pThis->m_callback.IsNull())
				{
					BYTE* bits = pThis->m_image.GetBits(pThis->m_index);
					TinySize size = pThis->m_image.GetSize();
					pThis->m_callback(bits, size.cx * size.cy * 4);
				}
				pThis->m_index = (++pThis->m_index) % count;
			}
			else
			{
				if (!pThis->m_callback.IsNull())
				{
					BYTE* bits = pThis->m_image.GetBits(0);
					TinySize size = pThis->m_image.GetSize();
					pThis->m_callback(bits, size.cx * size.cy * 4);
				}
			}
		}
	}

	BOOL MImageModel::Initialize(const CHAR* pzFile)
	{
		if (m_image.Load(pzFile))
		{
			TinySize imageSize = m_image.GetSize();
			HRESULT hRes = m_controller.GetD2D().GetContext()->CreateBitmap(D2D1::SizeU(imageSize.cx, imageSize.cy),
				(const void *)NULL,
				0,
				&D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
				&m_bitmap1);
			if (SUCCEEDED(hRes))
			{
				this->SetSize(imageSize);
				imageSize.cx = imageSize.cx / 2;
				imageSize.cy = imageSize.cy / 2;
				this->SetScale(imageSize);
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL MImageModel::Animate()
	{
		TinyTimerQueue& timers = TinyApplication::GetInstance()->GetTimers();
		if (m_hTimer != NULL)
		{
			timers.Unregister(m_hTimer);
			m_hTimer = NULL;
		}
		m_hTimer = timers.Register(&MImageModel::TimerCallback, this, m_image.GetDelay(m_index), 1, m_image.GetCount() > 1 ? WT_EXECUTEINTIMERTHREAD : WT_EXECUTEONLYONCE);
		return m_hTimer != NULL;
	}

	ID2D1Bitmap1* MImageModel::GetBitmap()
	{
		return m_bitmap1;
	}

	BOOL MImageModel::Draw()
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


	BOOL MImageModel::Release()
	{
		if (m_hTimer != NULL)
		{
			TinyApplication::GetInstance()->GetTimers().Unregister(m_hTimer);
			m_hTimer = NULL;
		}
		m_controller.Remove(this);
		m_bitmap1.Release();
		m_index = 0;
		return TRUE;
	}
}