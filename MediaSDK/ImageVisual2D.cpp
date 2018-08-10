#include "stdafx.h"
#include "ImageVisual2D.h"

namespace MediaSDK
{
	ImageVisual2D::ImageVisual2D(DX11& dx11)
		:m_dx11(dx11)
	{
	}

	ImageVisual2D::~ImageVisual2D()
	{
	}

	void ImageVisual2D::SetFile(const TinyString& szFile)
	{
		m_szFile = std::move(szFile);
	}

	BOOL ImageVisual2D::Open()
	{
		Close();
		if (!PathFileExists(m_szFile.CSTR()))
			return FALSE;
		return m_visual2D.Load(m_dx11, m_szFile.CSTR());
	}

	BOOL ImageVisual2D::Tick()
	{
		return TRUE;
	}

	void ImageVisual2D::Close()
	{
		m_visual2D.Destory();
	}

	LPCSTR ImageVisual2D::GetName()
	{
		return TEXT("ImageVisual2D");
	}

	DX11Image2D* ImageVisual2D::GetVisual2D()
	{
		return &m_visual2D;
	}

}
