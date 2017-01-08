#pragma once
#include "DX11Image.h"
#include "DX11CaptureTask.h"
using namespace DXFramework;

/// <summary>
/// ”Œœ∑≥°æ∞
/// </summary>
class GameScene : public DX11Element
{
public:
	GameScene();
	~GameScene();
	BOOL Initialize(DX11& dx11);
	LPCSTR GetClassName() OVERRIDE;
	BOOL BeginScene() OVERRIDE;
	void EndScene() OVERRIDE;
	BOOL Render(const DX11& dx11) OVERRIDE;
private:
	TinyScopedPtr<DX11CaptureTask>	m_captureTask;
};

