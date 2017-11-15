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

	BOOL MPreviewController::Open(LPCSTR pzURL)
	{
		TinySize size;
		m_player.Reset(new MFLVPlayer());
		if (!m_player)
			goto _ERROR;
		m_player->SetErrorCallback(BindCallback(&MPreviewController::OnError, this));
		if (!m_player->Open(m_view.Handle(), pzURL))
		{
			LOG(ERROR) << "[MPreviewController] " << "Open FAIL";
			goto _ERROR;
		}
		LOG(INFO) << "[MPreviewController] " << "Player Open OK";
		return TRUE;
	_ERROR:
		Close();
		return FALSE;
	}

	BOOL MPreviewController::Open(LPCSTR pzURL, Callback<void(BYTE*, LONG)>&& audioCB)
	{
		TinySize size;
		m_player.Reset(new MFLVPlayer(std::move(audioCB)));
		if (!m_player)
			goto _ERROR;
		m_player->SetErrorCallback(BindCallback(&MPreviewController::OnError, this));
		if (!m_player->Open(m_view.Handle(), pzURL))
		{
			LOG(ERROR) << "[MPreviewController] " << "Open FAIL";
			goto _ERROR;
		}
		LOG(INFO) << "[MPreviewController] " << "Player Open OK";
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
			TRACE("[MPreviewController] OnError:%d\n", iError);
			LOG(ERROR) << "[MPreviewController] OnError:" << iError;
		}
		break;
		}
	}
	void MPreviewController::OnTry()
	{
		BOOL bRes = this->Close();
		TRACE("[MPreviewController] OnTry Close:%d\n", bRes);
		LOG(INFO) << "[MPreviewController] OnTry Close:" << bRes;
		if (this->Open(m_player->GetURL().CSTR()))
		{
			TRACE("[MPreviewController] OnTry Open OK\n");
			LOG(INFO) << "[MPreviewController] OnTry Open OK";
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
	MFLVPlayer*	 MPreviewController::GetPlayer()
	{
		return m_player;
	}
}
