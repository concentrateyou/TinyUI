#include "stdafx.h"
#include "ScreenScene.h"
#include "Utility.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(ScreenScene, DX11Image);

	ScreenScene::ScreenScene()
		:m_bits(NULL),
		m_hBitmap(NULL)
	{
	}


	ScreenScene::~ScreenScene()
	{
	}

	BOOL ScreenScene::Initialize(DX11& dx11, const TinyRectangle& s)
	{
		Destory();
		if (!s.IsRectEmpty())
		{
			m_snapshot = s;
			if (DX11Image::Create(dx11, m_snapshot.Size(), NULL, FALSE))
			{
				SAFE_DELETE_OBJECT(m_hBitmap);
				BITMAPINFO bmi = { 0 };
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth = m_size.cx;
				bmi.bmiHeader.biHeight = -m_size.cy;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = m_size.cx * m_size.cy * 4;
				m_hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&m_bits), NULL, 0);
				m_memDC.Reset(new TinyMemDC(NULL, m_hBitmap));
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL ScreenScene::Process(DX11& dx11)
	{
		return TRUE;
	}
	void ScreenScene::Clear(DX11& dx11)
	{
		SAFE_DELETE_OBJECT(m_hBitmap);
		m_memDC.Reset(NULL);
	}
	BOOL ScreenScene::Render(DX11& dx11)
	{
		HDC hDC = ::GetDC(NULL);
		if (hDC != NULL)
		{
			/*TinyRectangle rectangle;
			GetClientRect(GetDesktopWindow(), &rectangle);
			TinyMemDC dc(hDC, TO_CX(rectangle), TO_CY(rectangle));;
			PaintDesktop(dc);*/
			INT cx = TO_CX(m_snapshot);
			INT cy = TO_CY(m_snapshot);
			::BitBlt(m_memDC->Handle(), 0, 0, cx, cy, hDC, m_snapshot.Position().x, m_snapshot.Position().y, SRCCOPY);
			UINT  linesize = cx * 4;
			this->Copy(dx11, m_bits, linesize * m_size.cy, linesize);
			::ReleaseDC(NULL, hDC);
			return DX11Image::Render(dx11);
		}
		return FALSE;
	}
}
