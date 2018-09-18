#include "stdafx.h"
#include "GameVisual2D.h"

namespace MediaSDK
{
	GameVisual2D::GameVisual2D(DX11& dx11)
		:m_dx11(dx11),
		m_captureRunner(&dx11, m_visual2D)
	{
		m_mutes[0].Create(FALSE, MUTEX_TEXTURE1, NULL);
		m_mutes[1].Create(FALSE, MUTEX_TEXTURE2, NULL);
	}

	GameVisual2D::~GameVisual2D()
	{
	}
	void GameVisual2D::Select(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
	{
		m_captureRunner.SetConfig(className, exeName, dllName);
	}

	XMFLOAT2 GameVisual2D::GetTranslate()
	{
		return m_visual2D.GetTranslate();
	}

	XMFLOAT2 GameVisual2D::GetScale()
	{
		return m_visual2D.GetScale();
	}

	XMFLOAT2 GameVisual2D::GetSize()
	{
		return m_visual2D.GetSize();
	}
	void GameVisual2D::SetName(LPCSTR pzName)
	{
		m_szname = pzName;
	}
	void GameVisual2D::SetScale(const XMFLOAT2& pos)
	{
		m_visual2D.SetScale(pos);
	}
	void GameVisual2D::SetTranslate(const XMFLOAT2& pos)
	{
		m_visual2D.SetTranslate(pos);
	}

	PCSTR GameVisual2D::GetName()
	{
		return m_szname.CSTR();
	}
	BOOL GameVisual2D::Open()
	{
		return m_captureRunner.Submit();
	}
	void GameVisual2D::Close()
	{
		m_captureRunner.Close();
	}
	BOOL GameVisual2D::Tick(INT64& timestamp)
	{
		if (!m_captureRunner.IsCapturing())
		{
			//TRACE("IsCapturing FAIL\n");
			return FALSE;
		}
		HookDATA* hookDATA = m_captureRunner.GetHookDATA();
		if (!hookDATA)
			return FALSE;
		switch (hookDATA->CaptureType)
		{
		case CAPTURETYPE_MEMORYTEXTURE:
		{
			SharedTextureDATA* pTextureDATA = m_captureRunner.GetSharedTextureDATA(hookDATA->MapSize);
			if (pTextureDATA != NULL)
			{
				DWORD dwCurrentID = pTextureDATA->CurrentID;
				BYTE* pBits = m_captureRunner.GetSharedTexture(hookDATA->MapSize);
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
								if (hookDATA->Pitch == ms.RowPitch)
								{
									memcpy(ms.pData, m_textures[dwCurrentID], hookDATA->Pitch * static_cast<INT>(size.y));
								}
								else
								{
									UINT bestPitch = std::min<UINT>(hookDATA->Pitch, ms.RowPitch);
									LPBYTE input = m_textures[dwCurrentID];
									for (INT y = 0; y < static_cast<INT>(size.y); y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (hookDATA->Pitch*y);
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
								if (hookDATA->Pitch == ms.RowPitch)
								{
									memcpy(ms.pData, m_textures[dwNextID], hookDATA->Pitch * static_cast<INT>(size.y));
								}
								else
								{
									UINT bestPitch = std::min<UINT>(hookDATA->Pitch, ms.RowPitch);
									LPBYTE input = m_textures[dwNextID];
									for (INT y = 0; y < static_cast<INT>(size.y); y++)
									{
										LPBYTE curInput = ((LPBYTE)input) + (hookDATA->Pitch * y);
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
				}
				return TRUE;
			}
		}
		break;
		case CAPTURETYPE_SHAREDTEXTURE:
			return TRUE;
		}
		return FALSE;
	}
	BOOL GameVisual2D::Draw(DX11Graphics2D& g)
	{
		FLOAT blendFactor[4] = { 0.0F, 0.0F, 0.0F, 0.0F };
		m_dx11.AllowBlend(FALSE, blendFactor);
		return g.DrawImage(m_visual2D);
	}
}