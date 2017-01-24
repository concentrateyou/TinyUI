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
		DECLARE_DYNAMIC(GameScene)
	public:
		GameScene();
		~GameScene();
		BOOL Initialize(DX11& dx11);
		void SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
	public:
		BOOL Process(DX11& dx11) OVERRIDE;
		BOOL Render(DX11& dx11) OVERRIDE;
		void Clear(DX11& dx11) OVERRIDE;
	private:
		TinyScopedPtr<DX11CaptureTask>	m_captureTask;
		TinyMutex						m_mute1;
		TinyMutex						m_mute2;
		BYTE*							m_textures[2];
	};

}

