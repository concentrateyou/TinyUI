#include "stdafx.h"
#include "MPreviewController.h"
#include "WICTexture.h"

namespace MShow
{
#define IDM_MOVEUP		100
#define IDM_MOVEDOWN	101
#define IDM_MOVETOP		102
#define IDM_MOVEBOTTPM	103
#define IDM_REMOVE		104

	MPreviewController::MPreviewController(MPreviewView& view)
		:m_view(view),
		m_hBitmap(NULL),
		m_pvBits(NULL)
	{

	}

	MPreviewController::~MPreviewController()
	{

	}

	BOOL MPreviewController::Initialize()
	{
		TinyRectangle s;
		GetClientRect(m_view.Handle(), &s);
		m_viewSize = s.Size();
		return TRUE;
	}

	BOOL MPreviewController::Open(LPCSTR pzURL)
	{
		BITMAPINFO bmi = { 0 };
		m_player.Reset(new MFLVPlayer(BindCallback(&MPreviewController::OnAudio, this), BindCallback(&MPreviewController::OnVideo, this)));
		if (!m_player)
			goto _ERROR;
		if (!m_player->Open(m_view.Handle(), pzURL))
			goto _ERROR;
		m_videoSize = m_player->GetSize();
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_videoSize.cx;
		bmi.bmiHeader.biHeight = -m_videoSize.cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = m_videoSize.cx * m_videoSize.cy * 4;
		m_hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&m_pvBits), NULL, 0);
		return m_hBitmap != NULL;
	_ERROR:
		if (m_player != NULL)
		{
			m_player->Close();
		}
		return FALSE;
	}

	BOOL MPreviewController::Close()
	{
		if (m_player != NULL)
		{
			m_player->Close();
		}
		SAFE_DELETE_OBJECT(m_hBitmap);
		return TRUE;
	}

	MPreviewView& MPreviewController::GetView()
	{
		return m_view;
	}
	LONGLONG MPreviewController::GetBasePTS()
	{
		ASSERT(m_player);
		return m_player->GetBasePTS();
	}
	void MPreviewController::OnAudio(BYTE* bits, LONG size)
	{

	}

	void MPreviewController::OnVideo(BYTE* bits, LONG size)
	{
		if (bits != NULL && size > 0 && m_hBitmap != NULL)
		{
			ASSERT(size == m_videoSize.cx * m_videoSize.cy * 4);
			memcpy(m_pvBits, bits, size);
			RECT rectangle = { 0 };
			::GetWindowRect(m_view.Handle(), &rectangle);
			TinyUI::TinyWindowDC wdc(m_view.Handle());
			TinyUI::TinyMemDC mdc(wdc, m_hBitmap);
			::SetStretchBltMode(wdc, HALFTONE);
			::StretchBlt(wdc, 0, 0, TO_CX(rectangle), TO_CY(rectangle), mdc, 0, 0, m_videoSize.cx, m_videoSize.cy, SRCCOPY);
		}
	}
}
