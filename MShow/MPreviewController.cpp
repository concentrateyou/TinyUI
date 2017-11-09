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
		if (!m_graphics.Initialize(m_view.Handle(), rectangle.Size()))
		{
			LOG(ERROR) << "[MPreviewController] " << "DX9Graphics2D Initialize FAIL";
			return FALSE;
		}
		LOG(INFO) << "[MPreviewController] " << "DX9Graphics2D Initialize OK";
		return TRUE;
	}

	BOOL MPreviewController::Open(LPCSTR pzURL)
	{
		TinySize size;
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
		size = m_player->GetSize();
		m_image.Reset(new DX9Image2D());
		if (!m_image)
			return FALSE;
		if (!m_image->Create(m_graphics.GetDX9(), size.cx, size.cy, NULL))
		{
			LOG(ERROR) << "[MPreviewController] " << "Create Image FAIL";
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
		if (m_image != NULL)
		{
			m_image->Destory();
		}
		m_image.Reset(NULL);
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
			m_timer.SetCallback(5000, BindCallback(&MPreviewController::OnTry, this));//ÿ��5������
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
	void MPreviewController::OnAudio(BYTE* bits, LONG size)
	{

	}
	BOOL MPreviewController::OnDraw(BYTE* bits, LONG size)
	{
		if (bits == NULL || size <= 0)
			return FALSE;
		if (!m_graphics.IsActive())
		{
			if (m_graphics.GetDX9().CheckReset())
			{
				if (m_graphics.Reset())
				{
					if (m_image != NULL)
					{
						m_image->Destory();
						TinySize videoSize = m_player->GetSize();
						if (!m_image->Create(m_graphics.GetDX9(), videoSize.cx, videoSize.cy, NULL))
						{
							LOG(ERROR) << "[MPreviewController] [OnDraw]" << " Create FAIL";
							return FALSE;
						}
					}
					else
					{
						m_image.Reset(new DX9Image2D());
						if (!m_image)
							return FALSE;
						TinySize videoSize = m_player->GetSize();
						if (!m_image->Create(m_graphics.GetDX9(), videoSize.cx, videoSize.cy, NULL))
						{
							LOG(ERROR) << "[MPreviewController] [OnDraw]" << " Create FAIL";
							return FALSE;
						}
					}
				}
			}
		}
		else
		{
			if (m_image != NULL)
			{
				if (!m_image->Copy(bits, size))
				{
					LOG(ERROR) << "[MPreviewController] [OnDraw]" << " Copy FAIL";
					return FALSE;
				}
				m_graphics.GetDX9().SetRenderTexture2D(NULL);
				if (!m_graphics.GetDX9().GetRender2D()->BeginDraw())
				{
					LOG(ERROR) << "[MPreviewController] [OnDraw]" << "BeginDraw FAIL";
					return FALSE;
				}
				if (!m_graphics.DrawImage(m_image))
				{
					LOG(ERROR) << "[MPreviewController] [OnDraw]" << "DrawImage FAIL";
					return FALSE;
				}
				if (!m_graphics.GetDX9().GetRender2D()->EndDraw())
				{
					LOG(ERROR) << "[MPreviewController] [OnDraw]" << "EndDraw FAIL";
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	void MPreviewController::OnVideoCopy(BYTE* bits, LONG size)
	{
		OnDraw(bits, size);
	}
	void MPreviewController::OnVideoRender()
	{
		m_graphics.Present();
	}
}
