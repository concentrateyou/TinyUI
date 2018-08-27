#include "stdafx.h"
#include "StreamVisual2D.h"

namespace MediaSDK
{
	StreamVisual2D::StreamVisual2D(DX11& dx11)
		:m_dx11(dx11)
	{

	}

	StreamVisual2D::~StreamVisual2D()
	{
	}

	void StreamVisual2D::SetFile(const TinyString& szFile)
	{
		if (PathFileExists(szFile.CSTR()))
		{

		}
	}

	XMFLOAT2 StreamVisual2D::GetTranslate()
	{
		return m_visual2D.GetTranslate();
	}

	XMFLOAT2 StreamVisual2D::GetScale()
	{
		return m_visual2D.GetScale();
	}

	XMFLOAT2 StreamVisual2D::GetSize()
	{
		return m_visual2D.GetSize();
	}

	void StreamVisual2D::SetTranslate(const XMFLOAT2& pos)
	{
		m_visual2D.SetTranslate(pos);
	}

	void StreamVisual2D::SetScale(const XMFLOAT2& pos)
	{
		m_visual2D.SetScale(pos);
	}

	BOOL StreamVisual2D::Open()
	{
		return TRUE;
	}

	BOOL StreamVisual2D::Tick()
	{
		return TRUE;
	}

	void StreamVisual2D::Close()
	{

	}

	LPCSTR StreamVisual2D::GetVisualName()
	{
		return TEXT("StreamVisual2D");
	}

	DX11Image2D* StreamVisual2D::GetVisual2D()
	{
		return &m_visual2D;
	}

}
