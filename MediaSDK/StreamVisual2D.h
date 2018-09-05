#pragma once
#include "MediaSDK.h"
#include "IVisual2D.h"
#include "VideoPacket.h"

namespace MediaSDK
{
	class StreamVisual2D : public IVisual2D
	{
		DECLARE_DYNAMIC(StreamVisual2D)
		DISALLOW_COPY_AND_ASSIGN(StreamVisual2D)
	public:
		StreamVisual2D(DX11& dx11);
		virtual ~StreamVisual2D();
		void	SetFile(const TinyString& szFile, const LAVVideoFormat& vF, const LAVAudioFormat& aF);
		static BOOL	QueryFormats(LPCSTR pzFile, vector<LAVVideoFormat>& videos, vector<LAVAudioFormat>& audios);
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Tick(INT64& timestamp) OVERRIDE;
		void			Close() OVERRIDE;
		LPCSTR			GetVisualName() OVERRIDE;
		DX11Image2D*	GetVisual2D() OVERRIDE;
		XMFLOAT2		GetTranslate() OVERRIDE;
		XMFLOAT2		GetSize() OVERRIDE;
		XMFLOAT2		GetScale() OVERRIDE;
		void			SetTranslate(const XMFLOAT2& pos) OVERRIDE;
		void			SetScale(const XMFLOAT2& pos) OVERRIDE;
	private:
		void			OnAudio(BYTE* bits, LONG size, REFERENCE_TIME timestamp);
		void			OnVideo(BYTE* bits, LONG size, REFERENCE_TIME timestamp);
	private:
		TinyString					m_szURL;
		LAVVideoFormat				m_vF;
		LAVAudioFormat				m_aF;
		TinyLock					m_lock;
		DX11&						m_dx11;
		DX11Image2D					m_visual2D;
		LAVWindowlessPlayer			m_player;
		TinyMemoryPool				m_pool;
		TinyLinkList<VideoSample>	m_samples;
	};
}


