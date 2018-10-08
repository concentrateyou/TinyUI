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
	void GameVisual2D::Select(const TinyString& className, const TinyString& exeName, FLOAT fFPS)
	{
		m_captureRunner.SetConfig(className, exeName, BindCallback(&GameVisual2D::OnCallback, this));
		UINT64 interval = static_cast<UINT64>(1000000000ULL / (fFPS * 2));
		m_captureRunner.SetInterval(interval);
	}
	void GameVisual2D::OnCallback()
	{
		UpdateRectangle(m_visual2D.GetTranslate(), m_visual2D.GetSize());
	}
	void GameVisual2D::UpdateRectangle(const XMFLOAT2& pos, const XMFLOAT2& size)
	{
		TinyRectangle s;
		s.SetRect(TinyPoint(static_cast<LONG>(pos.x), static_cast<LONG>(pos.y)), TinySize(static_cast<LONG>(size.x), static_cast<LONG>(size.y)));
		SetTrackerRect(s);
		OnChangedRect(&s);
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
		m_szNAME = pzName;
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
		return m_szNAME.CSTR();
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