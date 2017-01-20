#include "stdafx.h"
#include "VideoEncode.h"
#include "RenderTask.h"

namespace DXApp
{
	VideoEncode::VideoEncode(RenderTask* renderTask)
		:m_renderTask(renderTask)
	{
	}

	VideoEncode::~VideoEncode()
	{

	}

	x264Encode*	VideoEncode::GetEncode()
	{
		return &m_x264;
	}
	TinySize VideoEncode::GetSize() const
	{
		return	m_renderTask->GetGraphics().GetSize();
	}
	DWORD VideoEncode::GetFPS() const
	{
		return m_dwFPS;
	}
	BOOL VideoEncode::Encode()
	{
		ASSERT(m_renderTask);
		BYTE* bits = NULL;
		DWORD dwSize = 0;
		if (m_renderTask->GetGraphics().GetPointer(bits, dwSize))
		{
			if (m_converter->BRGAToI420(bits))
			{
				return m_x264.Encode(m_converter->GetI420());
			}
		}
		return FALSE;
	}

	BOOL VideoEncode::Close()
	{
		m_x264.Close();
		return TRUE;
	}

	BOOL VideoEncode::Open(const TinySize& scale, DWORD dwFPS, DWORD dwVideoRate)
	{
		ASSERT(m_renderTask);
		m_dwFPS = dwFPS;
		m_dwVideoRate = dwVideoRate;
		if (!m_x264.Open(scale.cx, scale.cy, (INT)dwFPS, (INT)dwVideoRate))
			return FALSE;
		m_converter.Reset(new I420Converter(GetSize(), scale));
		return TRUE;
	}
}

