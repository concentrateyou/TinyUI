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

	LPCSTR StreamVisual2D::GetName()
	{
		return TEXT("StreamVisual2D");
	}

	DX11Image2D* StreamVisual2D::GetVisual2D()
	{
		return &m_visual2D;
	}

}
