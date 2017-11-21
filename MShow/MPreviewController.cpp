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

	MPreviewView& MPreviewController::GetView()
	{
		return m_view;
	}

	MFLVPlayer*	 MPreviewController::GetPlayer()
	{
		return m_player;
	}
}
