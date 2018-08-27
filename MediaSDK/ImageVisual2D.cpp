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

	XMFLOAT2 ImageVisual2D::GetTranslate()
	{
		return m_visual2D.GetTranslate();
	}

	XMFLOAT2 ImageVisual2D::GetSize()
	{
		return m_visual2D.GetSize();
	}

	XMFLOAT2 ImageVisual2D::GetScale()
	{
		return m_visual2D.GetScale();
	}

	void ImageVisual2D::SetTranslate(const XMFLOAT2& pos)
	{
		m_visual2D.SetTranslate(pos);
	}
	void ImageVisual2D::SetScale(const XMFLOAT2& pos)
	{
		m_visual2D.SetScale(pos);
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

	LPCSTR ImageVisual2D::GetVisualName()
	{
		return TEXT("ImageVisual2D");
	}

	DX11Image2D* ImageVisual2D::GetVisual2D()
	{
		return &m_visual2D;
	}

}
