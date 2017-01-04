#pragma once
#include "VideoCapture.h"
#include "VideoCaptureParam.h"
#include "AudioCapture.h"
#include "AudioCaptureParam.h"
#include "DX11Image.h"
/// <summary>
/// ³¡¾°»ùÀà
/// </summary>
class BaseScene
{
public:
	BaseScene();
	~BaseScene();
public:
	virtual BOOL BeginScene() = 0;
	virtual BOOL EndScene() = 0;
	virtual BOOL Render(DX11& dx11) = 0;
	virtual BOOL Scale(INT cx, INT cy) = 0;
	virtual BOOL Move(INT x, INT y) = 0;
protected:
	TinyPerformanceTimer m_timer;
};

