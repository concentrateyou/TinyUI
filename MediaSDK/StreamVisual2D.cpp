#include "stdafx.h"
#include "StreamVisual2D.h"

namespace MediaSDK
{
	IMPLEMENT_DYNAMIC(StreamVisual2D, IVisual2D);

	StreamVisual2D::StreamVisual2D(DX11& dx11)
		:m_dx11(dx11)
	{

	}

	StreamVisual2D::~StreamVisual2D()
	{
	}

	void StreamVisual2D::SetFile(const TinyString& szFile)
	{
		m_szURL = szFile;
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
		m_player.EVENT_VIDEO += Delegate<void(BYTE*, LONG, REFERENCE_TIME)>(this, &StreamVisual2D::OnVideo);
		if (!m_player.Open(m_szURL.CSTR()))
			return FALSE;
		const VIDEOINFOHEADER& vih = m_player.GetVideoFormat();
		if (!m_visual2D.Create(m_dx11, vih.bmiHeader.biWidth, vih.bmiHeader.biHeight, NULL, FALSE))
			goto _ERROR;
		m_visual2D.SetFlipV(TRUE);
		if (!m_player.Play())
			goto _ERROR;
		return TRUE;
	_ERROR:
		m_player.Close();
		m_visual2D.Destory();
		return TRUE;
	}

	BOOL StreamVisual2D::Tick(INT64& timestamp)
	{
		/*TinyAutoLock autolock(m_lock);
		DWORD dwSize = m_ringBuffer.Read(m_buffer, 1);
		if (dwSize > 0)
		{
			if (!m_visual2D.Copy(m_dx11, m_buffer, m_linesize))
				return FALSE;
			return TRUE;
		}*/
		return TRUE;
	}

	void StreamVisual2D::Close()
	{
		m_player.Close();
		m_visual2D.Destory();
	}

	LPCSTR StreamVisual2D::GetVisualName()
	{
		return TEXT("StreamVisual2D");
	}

	DX11Image2D* StreamVisual2D::GetVisual2D()
	{
		return &m_visual2D;
	}

	void StreamVisual2D::OnVideo(BYTE* bits, LONG size, REFERENCE_TIME time)
	{
		const VIDEOINFOHEADER vih = m_player.GetVideoFormat();
		UINT linesize = LINESIZE(vih.bmiHeader.biBitCount, vih.bmiHeader.biWidth);
		VideoSample sample;
		ZeroMemory(&sample, sizeof(sample));
		
		/*	if (m_ringBuffer.IsEmpty())
			{
				m_buffer.Reset(linesize*vih.bmiHeader.biHeight);
				m_ringBuffer.Initialize(3, linesize*vih.bmiHeader.biHeight);
			}
			TinyAutoLock autolock(m_lock);
			m_ringBuffer.Write(bits, 1);*/
	}
}
