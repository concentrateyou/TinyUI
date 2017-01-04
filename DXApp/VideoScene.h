#pragma once
#include "BaseScene.h"
/// <summary>
/// …„œÒÕ∑≥°æ∞
/// </summary>
class VideoScene : public BaseScene
{
public:
	VideoScene();
	~VideoScene();
	BOOL Initialize(DX11& dx11, const VideoCapture::Name& name, const VideoCaptureParam& param);
	BOOL BeginScene() OVERRIDE;
	BOOL EndScene() OVERRIDE;
	BOOL Render(DX11& dx11) OVERRIDE;
	BOOL Scale(INT cx, INT cy) OVERRIDE;
	BOOL Move(INT x, INT y) OVERRIDE;
	DX11Image* GetImage() const;
private:
	TinyScopedPtr<DX11Image> m_image;
	VideoCapture	m_video;
	DX11&			m_dx11;
};

