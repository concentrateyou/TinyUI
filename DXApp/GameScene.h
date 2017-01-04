#pragma once
#include "BaseScene.h"

/// <summary>
/// ”Œœ∑≥°æ∞
/// </summary>
class GameScene : public BaseScene
{
public:
	GameScene();
	~GameScene();
	BOOL Initialize(DX11& dx11);
	virtual BOOL BeginScene() OVERRIDE;
	virtual BOOL EndScene() OVERRIDE;
	virtual BOOL Render(DX11& dx11) OVERRIDE;
	virtual BOOL Scale(INT cx, INT cy) OVERRIDE;
	virtual BOOL Move(INT x, INT y) OVERRIDE;
private:
	DX11Image						m_image;
	TinyScopedPtr<DX11CaptureTask>	m_captureTask;
};

