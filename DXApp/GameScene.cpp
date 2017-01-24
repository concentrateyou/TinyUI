#include "stdafx.h"
#include "GameScene.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(GameScene, DX11Image);
	GameScene::GameScene()
	{
		m_mute1.Create(FALSE, TEXTURE_MUTEX1, NULL);
		m_mute2.Create(FALSE, TEXTURE_MUTEX2, NULL);
	}


	GameScene::~GameScene()
	{
	}

	BOOL GameScene::Initialize(DX11& dx11)
	{
		m_captureTask.Reset(new DX11CaptureTask(&dx11, *this));
		return m_captureTask != NULL;
	}

	void GameScene::SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		m_captureTask->SetConfig(className, exeName, dllName);
	}

	BOOL GameScene::Process(DX11& dx11)
	{
		return m_captureTask->Submit();
	}

	void GameScene::Clear(DX11& dx11)
	{
		m_captureTask->Close();
		m_captureTask.Reset(NULL);
	}

	BOOL GameScene::Render(DX11& dx11)
	{
		ASSERT(m_captureTask);
		SharedCaptureDATA* pCaptureDATA = m_captureTask->GetSharedCaptureDATA();
		if (pCaptureDATA && pCaptureDATA->CaptureType == CAPTURETYPE_MEMORYTEXTURE)
		{
			SharedTextureDATA* pTextureDATA = m_captureTask->GetSharedTextureDATA(pCaptureDATA->MapSize);
			m_textures[0] = reinterpret_cast<LPBYTE>(pTextureDATA) + pTextureDATA->Texture1Offset;
			m_textures[1] = reinterpret_cast<LPBYTE>(pTextureDATA) + pTextureDATA->Texture2Offset;
			DWORD dwCurrentIndex = pTextureDATA->CurrentIndex;
			if (pTextureDATA && dwCurrentIndex < 2)
			{
				DWORD dwNextIndex = (dwCurrentIndex == 1) ? 0 : 1;
				do
				{
					if (m_mute1.Lock(0))
					{
						DX11Image::Copy(dx11, m_textures[dwCurrentIndex], pCaptureDATA->Pitch);
						m_mute1.Unlock();
						break;
					}
					if (m_mute2.Lock(0))
					{
						DX11Image::Copy(dx11, m_textures[dwCurrentIndex], pCaptureDATA->Pitch);
						m_mute2.Unlock();
						break;
					}
				} while (0);
			}
		}
		return DX11Image::Render(dx11);
	}
}
