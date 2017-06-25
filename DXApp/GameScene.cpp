#include "stdafx.h"
#include "GameScene.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(GameScene, DX11Image2D);

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

	BOOL GameScene::Allocate(DX11& dx11)
	{
		return m_captureTask->Submit();
	}

	void GameScene::Deallocate(DX11& dx11)
	{
		if (m_captureTask != NULL)
			m_captureTask->Close();
		m_captureTask.Reset(NULL);
	}

	BOOL GameScene::Draw(DX11& dx11)
	{
		ASSERT(m_captureTask);
		SharedCaptureDATA* pCaptureDATA = m_captureTask->GetSharedCaptureDATA();
		if (pCaptureDATA && pCaptureDATA->CaptureType == CAPTURETYPE_MEMORYTEXTURE)
		{
			SharedTextureDATA* pTextureDATA = m_captureTask->GetSharedTextureDATA(pCaptureDATA->MapSize);
			if (pTextureDATA)
			{
				DWORD dwCurrentID = pTextureDATA->CurrentID;
				BYTE* pBits = m_captureTask->GetSharedTexture(pCaptureDATA->MapSize);
				m_textures[0] = pBits + pTextureDATA->Texture1Offset;
				m_textures[1] = pBits + pTextureDATA->Texture2Offset;
				if (pTextureDATA)
				{
					do
					{
						DWORD dwNextID = (dwCurrentID == 1) ? 0 : 1;
						if (m_mutes[dwCurrentID].Lock(0))
						{
							BYTE *lpData;
							UINT iPitch;
							if (this->Map(dx11, lpData, iPitch))
							{
								if (pCaptureDATA->Pitch == iPitch)
									memcpy(lpData, m_textures[dwCurrentID], pCaptureDATA->Pitch * m_size.cy);
								else
								{
									UINT bestPitch = std::min<UINT>(pCaptureDATA->Pitch, iPitch);
									LPBYTE input = m_textures[dwCurrentID];
									for (INT y = 0; y < m_size.cy; y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (pCaptureDATA->Pitch*y);
										LPBYTE curOutput = ((LPBYTE)lpData) + (iPitch*y);
										memcpy(curOutput, curInput, bestPitch);
									}
								}
								this->Unmap(dx11);
							}
							m_mutes[dwCurrentID].Unlock();
							break;
						}
						if (m_mutes[dwNextID].Lock(0))
						{
							BYTE *lpData;
							UINT iPitch;
							if (this->Map(dx11, lpData, iPitch))
							{
								if (pCaptureDATA->Pitch == iPitch)
									memcpy(lpData, m_textures[dwNextID], pCaptureDATA->Pitch * m_size.cy);
								else
								{
									UINT bestPitch = std::min<UINT>(pCaptureDATA->Pitch, iPitch);
									LPBYTE input = m_textures[dwNextID];
									for (INT y = 0; y < m_size.cy; y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (pCaptureDATA->Pitch * y);
										LPBYTE curOutput = ((LPBYTE)lpData) + (iPitch*y);
										memcpy(curOutput, curInput, bestPitch);
									}
								}
								this->Unmap(dx11);
							}
							m_mutes[dwNextID].Unlock();
							break;
						}
					} while (0);
				}
			}
		}
		return DX11Image2D::Draw(dx11);
	}
}
