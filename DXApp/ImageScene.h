#pragma once
#include "DX11Image.h"
using namespace DXFramework;

namespace DXApp
{
	class ImageScene : public DX11Image
	{
		DECLARE_DYNAMIC(ImageScene)
	public:
		ImageScene();
		~ImageScene();
		BOOL Initialize(DX11& dx11, const TinyString& imageFile);
	public:
		BOOL Process(DX11& dx11) OVERRIDE;
		BOOL Render(DX11& dx11) OVERRIDE;
		void Clear(DX11& dx11) OVERRIDE;
	private:
		TinyString	m_imageFile;
	};

}


