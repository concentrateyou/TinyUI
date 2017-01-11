#include "stdafx.h"
#include "WindowScene.h"


WindowScene::WindowScene()
	:m_hWND(NULL)
{
}


WindowScene::~WindowScene()
{
}

BOOL WindowScene::Initialize(DX11& dx11, HWND hWND)
{
	m_hWND = hWND;
	Destory();
	TinyRectangle rectangle;
	GetWindowRect(m_hWND, &rectangle);
	return DX11Image::Create(dx11, rectangle.Size(), NULL);
}

LPCSTR WindowScene::GetClassName()
{
	return TEXT("WindowScene");
}

DXFramework::ElementType WindowScene::GetElementType() const
{
	return IMAGE;
}

BOOL WindowScene::BeginScene()
{
	return TRUE;
}

void WindowScene::EndScene()
{

}

BOOL WindowScene::Render(const DX11& dx11)
{
	HDC hDC = GetDC(m_hWND);
	if (hDC != NULL)
	{
		TinyRectangle rectangle;
		GetWindowRect(m_hWND, &rectangle);
		TinyPoint position = rectangle.Position();
		position.x = -position.x;
		position.y = -position.y;
		rectangle.OffsetRect(position);
		this->BitBlt(dx11, hDC, rectangle);
		DX11Image::Render(dx11);
		ReleaseDC(m_hWND, hDC);
		hDC = NULL;
		return TRUE;
	}
	return FALSE;
}
