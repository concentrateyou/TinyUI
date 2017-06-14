#include "stdafx.h"
#include "MFLVScene.h"
#include "MShowController.h"

namespace MShow
{
	MFLVScene::MFLVScene(MShowController* pController)
		:m_sample(0),
		m_bFI(0),
		m_pController(pController)
	{

	}

	MFLVScene::~MFLVScene()
	{
	}

	BOOL MFLVScene::Initialize(DX2D& d2d, LPCSTR pzURL)
	{
		m_audioTask.Reset(new MAudioTask(*this, m_clock));
		if (!m_audioTask)
			return FALSE;
		m_audioRenderTask.Reset(new MAudioRenderTask(*m_audioTask, m_clock));
		if (!m_audioRenderTask)
			return FALSE;
		m_videoTask.Reset(new MVideoTask(*this, m_clock));
		if (!m_videoTask)
			return FALSE;
		m_videoRenderTask.Reset(new MVideoRenderTask(*m_videoTask, m_clock, d2d, BindCallback(&MFLVScene::OnVideo, this)));
		if (!m_videoRenderTask)
			return FALSE;
		m_aac.Reset(new AACDecode());
		if (!m_aac)
			return FALSE;
		m_h264.Reset(new H264Decode());
		if (!m_h264)
			return FALSE;
		if (!m_reader.OpenURL(pzURL))
			return FALSE;
		m_script = m_reader.GetScript();
		if (!m_audioRenderTask->Initialize(d2d.GetHWND()))
			return FALSE;
		if (!m_videoRenderTask->Initialize())
			return FALSE;
		TinySize size(static_cast<LONG>(m_script.width), static_cast<LONG>(m_script.height));
		SetSize(size);
		SetScale(size);
		return TRUE;
	}

	BOOL MFLVScene::Submit()
	{
		m_close.CreateEvent();
		if (TinyTaskBase::Submit(BindCallback(&MFLVScene::OnMessagePump, this)))
		{
			if (!m_videoTask || !m_videoTask->Submit())
				return FALSE;
			if (!m_videoRenderTask || !m_videoRenderTask->Submit())
				return FALSE;
			if (!m_audioTask || !m_audioTask->Submit())
				return FALSE;
			if (!m_audioRenderTask || !m_audioRenderTask->Submit())
				return FALSE;
			return TRUE;
		}
		return FALSE;
	}

	BOOL MFLVScene::Draw(DX2D& d2d)
	{
		TinyPoint pos = GetPosition();
		TinySize size = GetScale();
		D2D_RECT_F dst = { static_cast<FLOAT>(pos.x),static_cast<FLOAT>(pos.y),static_cast<FLOAT>(pos.x + size.cx),static_cast<FLOAT>(pos.y + size.cy) };
		D2D_RECT_F src = { 0.0F,0.0F,static_cast<LONG>(m_size.cx), static_cast<LONG>(m_size.cy) };
		d2d.GetContext()->DrawBitmap(m_videoRenderTask->GetBitmap(), dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
		return TRUE;
	}

	BOOL MFLVScene::Close()
	{
		if (m_videoTask && m_videoTask->IsValid())
			m_videoTask->Close(INFINITE);
		if (m_videoRenderTask && m_videoRenderTask->IsValid())
			m_videoRenderTask->Close(INFINITE);
		if (m_audioTask && m_audioTask->IsValid())
			m_audioTask->Close(INFINITE);
		if (m_audioRenderTask && m_audioRenderTask->IsValid())
			m_audioRenderTask->Close(INFINITE);
		m_close.SetEvent();
		return TinyTaskBase::Close(INFINITE);
	}

	BOOL MFLVScene::SetVolume(LONG volume)
	{
		if (m_audioRenderTask)
			return m_audioRenderTask->SetVolume(volume);
		return FALSE;
	}


	MPacketQueue& MFLVScene::GetAudioQueue()
	{
		return m_audioQueue;
	}

	MPacketQueue& MFLVScene::GetVideoQueue()
	{
		return m_videoQueue;
	}

	H264Decode* MFLVScene::GetH264()
	{
		return m_h264;
	}
	AACDecode* MFLVScene::GetAAC()
	{
		return m_aac;
	}

	FLV_SCRIPTDATA& MFLVScene::GetScript()
	{
		return m_script;
	}

	void ReleaseBlock(FLV_BLOCK& block)
	{
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}

	void MFLVScene::OnMessagePump()
	{
		SampleTag tag = { 0 };
		FLV_BLOCK block = { 0 };
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			INT size = m_audioQueue.GetSize() + m_videoQueue.GetSize();
			if (size > MAX_QUEUE_SIZE)
			{
				Sleep(15);
				continue;
			}
			if (!m_reader.ReadBlock(block))
			{
				ReleaseBlock(block);
				goto _ERROR;
			}

			if (block.type == FLV_AUDIO)
			{
				if (block.audio.codeID == FLV_CODECID_AAC)
				{
					if (block.audio.packetType == FLV_AudioSpecificConfig)
					{
						if (!m_aac->Open(block.audio.data, block.audio.size, block.audio.bitsPerSample == 0 ? 8 : 16))
						{
							ReleaseBlock(block);
							goto _ERROR;
						}
						ReleaseBlock(block);
					}
					if (block.audio.packetType == FLV_AACRaw)
					{
						if (!m_bFI)//等待关键帧
						{
							ReleaseBlock(block);
							continue;
						}
						ZeroMemory(&tag, sizeof(tag));
						tag.bits = block.audio.data;
						tag.size = block.audio.size;
						tag.sample = ++m_sample;
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						m_audioQueue.Push(tag);
					}
				}
			}
			if (block.type == FLV_VIDEO)
			{
				if (block.video.codeID == FLV_CODECID_H264)
				{
					if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
					{
						TinySize size(static_cast<LONG>(m_script.width), static_cast<LONG>(m_script.height));
						if (!m_h264->Initialize(size, size))
						{
							ReleaseBlock(block);
							goto _ERROR;
						}
						if (!m_h264->Open(block.video.data, block.video.size))
						{
							ReleaseBlock(block);
							goto _ERROR;
						}
						ReleaseBlock(block);
					}
					if (block.video.packetType == FLV_NALU)
					{
						if (!m_bFI)//等待关键帧
						{
							if (block.video.codeType != 1)
							{
								ReleaseBlock(block);
								continue;
							}
							m_bFI = TRUE;
						}
						ZeroMemory(&tag, sizeof(tag));
						tag.sample = ++m_sample;
						tag.bits = block.video.data;
						tag.size = block.video.size;
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						m_videoQueue.Push(tag);
					}
				}
			}
		}
	_ERROR:
		m_audioQueue.RemoveAll();
		m_videoQueue.RemoveAll();
	}

	void MFLVScene::OnVideo(ID2D1Bitmap1* bitmap, INT delay)
	{
		ASSERT(m_pController);
		m_pController->Draw(this);
	}
}
