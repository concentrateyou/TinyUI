#include "stdafx.h"
#include "GameScene.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(GameScene, DX11Image);
	GameScene::GameScene()
	{
		m_mutes[0].Create(FALSE, TEXTURE_MUTEX1, NULL);
		m_mutes[1].Create(FALSE, TEXTURE_MUTEX2, NULL);
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
			if (pTextureDATA)
			{
				DWORD dwCurrentMutex = pTextureDATA->CurrentMutex;
				BYTE* pBits = m_captureTask->GetSharedTexture(pCaptureDATA->MapSize);
				m_textures[0] = pBits + pTextureDATA->Texture1Offset;
				m_textures[1] = pBits + pTextureDATA->Texture2Offset;
				if (pTextureDATA)
				{
					do
					{
						DWORD dwNextMutex = (dwCurrentMutex == 1) ? 0 : 1;
						if (m_mutes[dwCurrentMutex].Lock(0))
						{
							/*BYTE *lpData;
							UINT iPitch;
							if (Map(lpData, iPitch))
							{
								if (pCaptureDATA->Pitch == iPitch)
									memcpy(lpData, m_textures[dwCurrentMutex], pitch*height);
								else
								{
									UINT bestPitch = MIN(pitch, iPitch);
									LPBYTE input = textureBuffers[curTexture];
									for (UINT y = 0; y < height; y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (pitch*y);
										LPBYTE curOutput = ((LPBYTE)lpData) + (iPitch*y);

										memcpy(curOutput, curInput, bestPitch);
									}
								}

								texture->Unmap();
							}*/
							//DX11Image::Copy(dx11, m_textures[dwCurrentMutex], pCaptureDATA->Pitch);
							m_mutes[dwCurrentMutex].Unlock();
							break;
						}
						if (m_mutes[dwNextMutex].Lock(0))
						{
							//DX11Image::Copy(dx11, m_textures[dwNextMutex], pCaptureDATA->Pitch);
							m_mutes[dwCurrentMutex].Unlock();
							//curTexture = nextTexture;
							break;
						}
					} while (0);
				}
			}
		}
		return DX11Image::Render(dx11);
	}
}
