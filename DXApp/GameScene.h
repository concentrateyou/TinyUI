#pragma once
#include "DX11Image.h"
#include "DX11CaptureTask.h"
using namespace DXFramework;

namespace DXApp
{
	/// <summary>
	/// ”Œœ∑≥°æ∞
	/// </summary>
	class GameScene : public DX11Image
	{
	public:
		GameScene();
		~GameScene();
		BOOL Initialize(DX11& dx11);
		void SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
		LPCSTR GetClassName() OVERRIDE;
		BOOL BeginScene() OVERRIDE;
		void EndScene() OVERRIDE;
		BOOL Render(DX11& dx11) OVERRIDE;
	private:
		TinyScopedPtr<DX11CaptureTask>	m_captureTask;
	};

}

