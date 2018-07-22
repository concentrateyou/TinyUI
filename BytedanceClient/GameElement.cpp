#include "stdafx.h"
#include "GameElement.h"

namespace Bytedance
{
	IMPLEMENT_DYNAMIC(GameElement, DX10Image2D);

	GameElement::GameElement(DX10& dx10)
		:m_dx10(dx10),
		m_captureRunner(dx10, *this)
	{
		m_mutes[0].Create(FALSE, TEXTURE_MUTEX1, NULL);
		m_mutes[1].Create(FALSE, TEXTURE_MUTEX2, NULL);
	}

	GameElement::~GameElement()
	{
	}
	void GameElement::Select(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		m_captureRunner.SetConfig(className, exeName, dllName);
	}
	PCSTR GameElement::GetName()
	{
		return TEXT("GameElement");
	}
	DX11Element* GameElement::GetVisual()
	{
		return NULL;
	}
	BOOL GameElement::Open()
	{
		return m_captureRunner.Submit();
	}
	BOOL GameElement::Close()
	{
		return m_captureRunner.Close();
	}

	BOOL GameElement::Process()
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
							D3D10_MAPPED_TEXTURE2D ms;
							if (this->Map(ms))
							{
								if (pCaptureDATA->Pitch == ms.RowPitch)
									memcpy(ms.pData, m_textures[dwCurrentID], pCaptureDATA->Pitch * static_cast<INT>(m_size.y));
								else
								{
									UINT bestPitch = std::min<UINT>(pCaptureDATA->Pitch, ms.RowPitch);
									LPBYTE input = m_textures[dwCurrentID];
									for (INT y = 0; y < static_cast<INT>(m_size.y); y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (pCaptureDATA->Pitch*y);
										LPBYTE curOutput = ((LPBYTE)ms.pData) + (ms.RowPitch*y);
										memcpy(curOutput, curInput, bestPitch);
									}
								}
								this->Unmap();
							}
							m_mutes[dwCurrentID].Unlock();
							break;
						}
						if (m_mutes[dwNextID].Lock(0))
						{
							D3D10_MAPPED_TEXTURE2D ms;
							if (this->Map(ms))
							{
								if (pCaptureDATA->Pitch == ms.RowPitch)
									memcpy(ms.pData, m_textures[dwNextID], pCaptureDATA->Pitch * static_cast<INT>(m_size.y));
								else
								{
									UINT bestPitch = std::min<UINT>(pCaptureDATA->Pitch, ms.RowPitch);
									LPBYTE input = m_textures[dwNextID];
									for (INT y = 0; y < static_cast<INT>(m_size.y); y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (pCaptureDATA->Pitch * y);
										LPBYTE curOutput = ((LPBYTE)ms.pData) + (ms.RowPitch*y);
										memcpy(curOutput, curInput, bestPitch);
									}
								}
								this->Unmap();
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
