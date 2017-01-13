#pragma once
#include "DX11Image.h"
#include "DX11.h"
using namespace DXFramework;

namespace DXApp
{
	class ScreenScene : public DX11Image
	{
	public:
		ScreenScene();
		virtual ~ScreenScene();
		BOOL Initialize(DX11& dx11, const TinyRectangle& s);
	public:
		LPCSTR GetClassName() OVERRIDE;
		ElementType GetElementType() const OVERRIDE;
		BOOL BeginScene() OVERRIDE;
		void EndScene() OVERRIDE;
		BOOL Render(const DX11& dx11) OVERRIDE;
	private:
		TinyRectangle	m_snapshot;
	};

}


