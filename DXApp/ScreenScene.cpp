#include "stdafx.h"
#include "ScreenScene.h"
#include "Utility.h"
#include <VersionHelpers.h>

namespace DXApp
{
	IMPLEMENT_DYNAMIC(ScreenScene, DX11Image2D);

	ScreenScene::ScreenScene()
		:m_bits(NULL),
		m_hBitmap(NULL)
	{
	}


	ScreenScene::~ScreenScene()
	{
	}

	BOOL ScreenScene::Initialize(DX11& dx11, const TinyRectangle& snapshot)
	{
		Destory();
		m_snapshot = snapshot;
		if (!m_snapshot.IsRectEmpty())
		{
			if (IsWindows8OrGreater())
			{
				UINT count = m_duplicator.GetOutputs(dx11);
				if (count > 0 &&
					m_duplicator.Initialize(dx11, 0))
				{
					if (DX11Image2D::Load(dx11, m_duplicator.GetSharedHandle()))
					{
						return TRUE;
					}
				}
			}
			else
			{
				if (DX11Image2D::Create(dx11, m_snapshot.Size(), NULL, FALSE))
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
		if (IsWindows8OrGreater())
		{
			m_duplicator.AcquireNextFrame(dx11, 100);
		}
		else
		{
			HDC hDC = ::GetDC(NULL);
			if (hDC != NULL)
			{
				INT cx = TO_CX(m_snapshot);
				INT cy = TO_CY(m_snapshot);
				::BitBlt(m_memDC->Handle(), 0, 0, cx, cy, hDC, m_snapshot.Position().x, m_snapshot.Position().y, SRCCOPY);
				CURSORINFO ci;
				ZeroMemory(&ci, sizeof(ci));
				ci.cbSize = sizeof(ci);
				if (GetCursorInfo(&ci))
				{
					POINT pos = ci.ptScreenPos;
					ScreenToClient(NULL, &pos);
					if (ci.flags & CURSOR_SHOWING)
					{
						HICON hICON = CopyIcon(ci.hCursor);
						if (hICON != NULL)
						{
							DrawIcon(m_memDC->Handle(), pos.x - m_snapshot.Position().x, pos.y - m_snapshot.Position().y, hICON);
							DestroyIcon(hICON);
						}
					}
				}
				UINT  linesize = cx * 4;
				this->Copy(dx11, m_bits, linesize * m_size.cy, linesize);
				::ReleaseDC(NULL, hDC);
			}
		}
		return DX11Image2D::Render(dx11);
	}
}
