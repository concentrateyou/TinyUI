#include "stdafx.h"
#include "GameVisual2D.h"

namespace MediaSDK
{
	GameVisual2D::GameVisual2D(DX11& dx11)
		:m_dx11(dx11),
		m_captureRunner(&dx11, m_visual2D)
	{
		m_mutes[0].Create(FALSE, TEXTURE_MUTEX1, NULL);
		m_mutes[1].Create(FALSE, TEXTURE_MUTEX2, NULL);
	}

	GameVisual2D::~GameVisual2D()
	{
	}
	void GameVisual2D::Select(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		m_captureRunner.SetConfig(className, exeName, dllName);
	}
	PCSTR GameVisual2D::GetName()
	{
		return TEXT("GameVisual2D");
	}
	DX11Image2D* GameVisual2D::GetVisual2D()
	{
		return NULL;
	}
	BOOL GameVisual2D::Open()
	{
		return m_captureRunner.Submit();
	}
	void GameVisual2D::Close()
	{
		m_captureRunner.Close();
	}
	BOOL GameVisual2D::Process()
	{
		SharedCaptureDATA* pCaptureDATA = m_captureRunner.GetSharedCaptureDATA();
		if (pCaptureDATA != NULL && pCaptureDATA->CaptureType == CAPTURETYPE_MEMORYTEXTURE)
		{
			SharedTextureDATA* pTextureDATA = m_captureRunner.GetSharedTextureDATA(pCaptureDATA->MapSize);
			if (pTextureDATA != NULL)
			{
				DWORD dwCurrentID = pTextureDATA->CurrentID;
				BYTE* pBits = m_captureRunner.GetSharedTexture(pCaptureDATA->MapSize);
				m_textures[0] = pBits + pTextureDATA->Texture1Offset;
				m_textures[1] = pBits + pTextureDATA->Texture2Offset;
				if (pTextureDATA != NULL)
				{
					do
					{
						DWORD dwNextID = (dwCurrentID == 1) ? 0 : 1;
						if (m_mutes[dwCurrentID].Lock(0))
						{
							D3D11_MAPPED_SUBRESOURCE ms;
							if (m_visual2D.Map(m_dx11, ms, FALSE))
							{
								XMFLOAT2 size = m_visual2D.GetSize();
								if (pCaptureDATA->Pitch == ms.RowPitch)
									memcpy(ms.pData, m_textures[dwCurrentID], pCaptureDATA->Pitch * static_cast<INT>(size.y));
								else
								{
									UINT bestPitch = std::min<UINT>(pCaptureDATA->Pitch, ms.RowPitch);
									LPBYTE input = m_textures[dwCurrentID];
									for (INT y = 0; y < static_cast<INT>(size.y); y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (pCaptureDATA->Pitch*y);
										LPBYTE curOutput = ((LPBYTE)ms.pData) + (ms.RowPitch*y);
										memcpy(curOutput, curInput, bestPitch);
									}
								}
								m_visual2D.Unmap(m_dx11);
							}
							m_mutes[dwCurrentID].Unlock();
							break;
						}
						if (m_mutes[dwNextID].Lock(0))
						{
							D3D11_MAPPED_SUBRESOURCE ms;
							if (m_visual2D.Map(m_dx11, ms, FALSE))
							{
								XMFLOAT2 size = m_visual2D.GetSize();
								if (pCaptureDATA->Pitch == ms.RowPitch)
									memcpy(ms.pData, m_textures[dwNextID], pCaptureDATA->Pitch * static_cast<INT>(size.y));
								else
								{
									UINT bestPitch = std::min<UINT>(pCaptureDATA->Pitch, ms.RowPitch);
									LPBYTE input = m_textures[dwNextID];
									for (INT y = 0; y < static_cast<INT>(size.y); y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (pCaptureDATA->Pitch * y);
										LPBYTE curOutput = ((LPBYTE)ms.pData) + (ms.RowPitch*y);
										memcpy(curOutput, curInput, bestPitch);
									}
								}
								m_visual2D.Unmap(m_dx11);
							}
							m_mutes[dwNextID].Unlock();
							break;
						}
					} while (0);
					return TRUE;
				}
			}
		}
		return FALSE;
	}
}