#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11TextureShader.h"

namespace DXFramework
{
	class DX11Element
	{
	public:
		DX11Element();
		~DX11Element();
	public:
		void SetPosition(const TinyPoint& pos);
		TinyPoint GetPosition() const;
		void SetScale(const TinySize& size);
		TinySize GetScale() const;
		virtual LPCSTR GetClassName() = 0;
		virtual BOOL BeginScene() = 0;
		virtual void EndScene() = 0;
		virtual BOOL Render(const DX11& dx11) = 0;
	protected:
		TinySize					m_scale;
		TinyPoint					m_position;
	};
}



