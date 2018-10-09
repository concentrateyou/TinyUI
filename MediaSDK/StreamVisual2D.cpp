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
	BOOL StreamVisual2D::QueryFormats(LPCSTR pzFile, vector<LAVVideoFormat>& videos, vector<LAVAudioFormat>& audios)
	{
		return LAVWindowlessPlayer::GetFormats(pzFile, videos, audios);
	}
	void StreamVisual2D::SetFile(const TinyString& szFile, const LAVVideoFormat& vF, const LAVAudioFormat& aF)
	{
		m_szURL = szFile;
		m_vF = vF;
		m_aF = aF;
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
	void StreamVisual2D::SetName(LPCSTR pzName)
	{
		m_szNAME = pzName;
	}
	BOOL StreamVisual2D::Open()
	{
		m_player.EVENT_AUDIO += Delegate<void(BYTE*, LONG, REFERENCE_TIME)>(this, &StreamVisual2D::OnAudio);
		m_player.EVENT_VIDEO += Delegate<void(BYTE*, LONG, REFERENCE_TIME)>(this, &StreamVisual2D::OnVideo);
		if (!m_player.Open(m_szURL.CSTR(), m_vF, m_aF))
			return FALSE;
		const LAVVideoFormat& vF = m_player.GetVideoFormat();
		TinySize sizeT = vF.GetSize();
		if (!m_visual2D.Create(m_dx11, sizeT.cx, sizeT.cy, DXGI_FORMAT_B8G8R8A8_UNORM, NULL, FALSE))
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
		return TRUE;
	}
	BOOL StreamVisual2D::Draw(DX11Graphics2D& g)
	{
		return TRUE;
	}

	void StreamVisual2D::Close()
	{
		m_player.Close();
		m_visual2D.Destory();
	}

	LPCSTR StreamVisual2D::GetName()
	{
		return TEXT("StreamVisual2D");
	}

	void StreamVisual2D::OnAudio(BYTE* bits, LONG size, REFERENCE_TIME timestamp)
	{
		const LAVAudioFormat& aF = m_player.GetAudioFormat();

	}
	void StreamVisual2D::OnVideo(BYTE* bits, LONG size, REFERENCE_TIME timestamp)
	{
		const LAVVideoFormat& vF = m_player.GetVideoFormat();
		TinySize sizeT = vF.GetSize();
		VideoPacket sample;
		ZeroMemory(&sample, sizeof(sample));
		sample.cx = static_cast<UINT32>(sizeT.cx);
		sample.cy = static_cast<UINT32>(sizeT.cy);
		sample.format = vF.GetFormat();
		sample.timestamp = timestamp;
		switch (sample.format)
		{
		case PIXEL_FORMAT_RGB32:
			sample.data[0] = bits;
			sample.linesize[0] = sample.cx * 4;
			break;
		case PIXEL_FORMAT_YUY2:
		case PIXEL_FORMAT_UYVY:
			sample.data[0] = bits;
			sample.linesize[0] = sample.cx * 2;
			break;
		case PIXEL_FORMAT_I420:
			sample.data[0] = bits;
			sample.data[1] = sample.data[0] + (sample.cx * sample.cy);
			sample.data[2] = sample.data[1] + (sample.cx * sample.cy / 4);
			sample.linesize[0] = sample.cx;
			sample.linesize[1] = sample.cx / 2;
			sample.linesize[2] = sample.cx / 2;
			break;
		case PIXEL_FORMAT_YV12:
			sample.data[0] = bits;
			sample.data[2] = sample.data[0] + (sample.cx *  sample.cy);
			sample.data[1] = sample.data[2] + (sample.cx *  sample.cy / 4);
			sample.linesize[0] = sample.cx;
			sample.linesize[1] = sample.cx / 2;
			sample.linesize[2] = sample.cx / 2;
			break;
		case PIXEL_FORMAT_NV12:
			sample.data[0] = bits;
			sample.data[1] = sample.data[0] + (sample.cx *  sample.cy);
			sample.linesize[0] = sample.cx;
			sample.linesize[1] = sample.cx;
			break;
		default:
			return;
		}
		TinyAutoLock autolock(m_lock);
		m_samples.InsertLast(sample);
	}
}
