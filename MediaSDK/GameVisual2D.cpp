#include "stdafx.h"
#include "GameVisual2D.h"

namespace MediaSDK
{
	GameVisual2D::GameVisual2D(DX11& dx11)
		:m_dx11(dx11),
		m_captureRunner(&dx11, m_visual2D)
	{

	}
	GameVisual2D::~GameVisual2D()
	{
	}
	void GameVisual2D::Select(const TinyString& className, const TinyString& exeName)
	{
		m_captureRunner.SetConfig(className, exeName);
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
		INT64 timestamp = 0;
		return Tick(timestamp);
	}
	void GameVisual2D::Close()
	{
		m_captureRunner.Close();
	}
	BOOL GameVisual2D::Tick(INT64& timestamp)
	{
		return	m_captureRunner.Tick(timestamp);
	}
	BOOL GameVisual2D::Draw(DX11Graphics2D& g)
	{
		FLOAT blendFactor[4] = { 0.0F, 0.0F, 0.0F, 0.0F };
		m_dx11.AllowBlend(FALSE, blendFactor);
		return g.DrawImage(m_visual2D);
	}
}