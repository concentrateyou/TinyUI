#pragma once
#include "DX11Image.h"
#include "DX11.h"
using namespace DXFramework;

class WindowScene : public DX11Image
{
public:
	WindowScene();
	virtual ~WindowScene();
	BOOL Initialize(DX11& dx11, HWND hWND);
public:
	LPCSTR GetClassName() OVERRIDE;
	ElementType GetElementType() const OVERRIDE;
	BOOL BeginScene() OVERRIDE;
	void EndScene() OVERRIDE;
	BOOL Render(const DX11& dx11) OVERRIDE;
private:
	HWND m_hWND;
};

