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
		:m_view(view)
	{

	}

	MPreviewController::~MPreviewController()
	{

	}

	BOOL MPreviewController::Initialize()
	{
		TinyRectangle rectangle;
		GetClientRect(m_view.Handle(), &rectangle);
		if (!m_d2d.Initialize(m_view.Handle(), rectangle.Size().cx, rectangle.Size().cy))
		{
			LOG(ERROR) << "[MPreviewController] " << "D2D Initialize FAIL";
			return FALSE;
		}
		LOG(INFO) << "[MPreviewController] " << "D2D Initialize OK";
		return TRUE;
	}

	BOOL MPreviewController::Open(LPCSTR pzURL)
	{
		m_player.Reset(new MFLVPlayer(BindCallback(&MPreviewController::OnAudio, this), BindCallback(&MPreviewController::OnVideoCopy, this), BindCallback(&MPreviewController::OnVideoRender, this)));
		if (!m_player)
			goto _ERROR;
		m_player->SetErrorCallback(BindCallback(&MPreviewController::OnError, this));
		if (!m_player->Open(pzURL))
		{
			LOG(ERROR) << "[MPreviewController] " << "Open FAIL";
			goto _ERROR;
		}
		LOG(INFO) << "[MPreviewController] " << "Player Open OK";
		if (!CreateBitmap(m_player->GetSize()))
		{
			LOG(ERROR) << "[MPreviewController] " << "CreateBitmap FAIL";
			goto _ERROR;
		}
		return TRUE;
	_ERROR:
		Close();
		return FALSE;
	}

	BOOL MPreviewController::Close()
	{
		if (m_player != NULL)
		{
			m_player->Close();
		}
		m_bitmap.Release();
		LOG(INFO) << "[MPreviewController] " << "Player Close OK";
		return TRUE;
	}

	void MPreviewController::OnError(INT iError)
	{
		switch (iError)
		{
		case WSAENETDOWN:
		case WSAENETUNREACH:
		case WSAENETRESET:
		case WSAECONNABORTED:
		case WSAECONNRESET:
		{
			m_timer.SetCallback(5000, BindCallback(&MPreviewController::OnTry, this));//Ã¿¸ô5ÃëÖØÊÔ
			TRACE("On Error:%d\n", iError);
		}
		break;
		}
	}
	void MPreviewController::OnTry()
	{
		BOOL bRes = this->Close();
		TRACE("OnTry Close:%d\n", bRes);
		LOG(INFO) << "OnTry Close:" << bRes;
		if (this->Open(m_player->GetURL().CSTR()))
		{
			TRACE("OnTry Open OK\n");
			LOG(INFO) << "OnTry Open OK";
			m_timer.Close();
		}
		else
		{
			TRACE("OnTry Open FAIL\n");
			LOG(ERROR) << "OnTry Open FAIL";
		}
	}

	MPreviewView& MPreviewController::GetView()
	{
		return m_view;
	}
	LONGLONG MPreviewController::GetBasePTS()
	{
		if (m_player != NULL)
			return m_player->GetBasePTS();
		LOG(ERROR) << "[MPreviewController] " << "GetBasePTS NULL";
		return  0;
	}
	BOOL MPreviewController::CreateBitmap(const TinySize& size)
	{
		m_bitmap.Release();
		HRESULT hRes = m_d2d.GetCanvas()->CreateBitmap(D2D1::SizeU(size.cx, size.cy), D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &m_bitmap);
		if (FAILED(hRes))
		{
			LOG(ERROR) << "[MPreviewController] " << "CreateBitmap FAIL";
			return FALSE;
		}
		LOG(INFO) << "[MPreviewController] " << "CreateBitmap OK";
		return TRUE;
	}
	void MPreviewController::OnAudio(BYTE* bits, LONG size)
	{

	}

	void MPreviewController::OnVideoCopy(BYTE* bits, LONG size)
	{
		if (m_bitmap != NULL && bits != NULL && size > 0)
		{
			UINT s = m_player->GetSize().cx * 4;
			HRESULT hRes = m_bitmap->CopyFromMemory(NULL, bits, s);
			if (hRes != S_OK)
			{
				LOG(ERROR) << "[MPreviewController] " << "CopyFromMemory FAIL";
			}
		}
	}
	void MPreviewController::OnVideoRender()
	{
		HRESULT hRes = S_OK;
		if (m_d2d.BeginDraw())
		{
			m_d2d.GetCanvas()->DrawBitmap(m_bitmap);
			if (!m_d2d.EndDraw(hRes))
			{
				LOG(ERROR) << "[MPreviewController] EndDraw FAIL: " << hRes;
			}
			else
			{
				if (hRes == D2DERR_RECREATE_TARGET)
				{
					if (CreateBitmap(m_player->GetSize()))
					{
						LOG(INFO) << "[MPreviewController] OnVideoRender - CreateBitmap OK";
					}
					else
					{
						LOG(ERROR) << "[MPreviewController] OnVideoRender - CreateBitmap FAIL";
					}
				}
			}
		}
	}
}
