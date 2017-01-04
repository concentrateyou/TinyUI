#include "stdafx.h"
#include "GameScene.h"


GameScene::GameScene()
{
}


GameScene::~GameScene()
{
}

BOOL GameScene::Initialize(DX11& dx11)
{
	m_captureTask.Reset(new DX11CaptureTask(&dx11));
	return m_captureTask != NULL;
}

BOOL GameScene::BeginScene()
{
	return m_captureTask->Submit();
}

BOOL GameScene::EndScene()
{
	return m_captureTask->Close(INFINITE);
}

BOOL GameScene::Render(DXGraphics& graphics)
{

}

BOOL GameScene::Scale(INT cx, INT cy)
{
	m_image.SetScale(cx, cy);
	return TRUE;
}
BOOL GameScene::Move(INT x, INT y)
{
	m_image.SetPosition(x, y);
	return TRUE;
}
