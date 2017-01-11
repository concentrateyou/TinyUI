#pragma once
#include "DX11Image.h"
using namespace DXFramework;

namespace DXApp
{
	class ImageScene : public DX11Image
	{
	public:
		ImageScene();
		~ImageScene();
		BOOL Initialize(DX11& dx11, const TinyString& imageFile);
		LPCSTR GetClassName() OVERRIDE;
		BOOL BeginScene() OVERRIDE;
		void EndScene() OVERRIDE;
		BOOL Render(const DX11& dx11) OVERRIDE;
	private:
		TinyString	m_imageFile;
	};

}


