#include "stdafx.h"
#include "QSVTask.h"
#include "QSVDecode.h"

namespace FLVPlayer
{
	QSVTask::QSVTask()
		:m_hFile(NULL),
		m_pYUV420(NULL)
	{

	}
	QSVTask::~QSVTask()
	{
		fclose(m_hFile);
	}
	BOOL QSVTask::Initialize(HWND hWND)
	{
		if (!m_reader.OpenFile("D:\\3.flv"))
			return FALSE;
		TinySize size(static_cast<LONG>(m_reader.GetScript().width), static_cast<LONG>(m_reader.GetScript().height));
		TinyRectangle rectangle;
		::GetClientRect(hWND, &rectangle);
		if (!m_graphics.Initialize(hWND, rectangle.Size()))
			return FALSE;
		if (!m_video2D.Create(m_graphics.GetDX11(), size, TRUE))
			return FALSE;
		m_video2D.SetScale(rectangle.Size());

		m_pYUV420 = av_frame_alloc();
		if (!m_pYUV420)
			return FALSE;
		INT val = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, size.cx, size.cy, 1);
		m_bits.Reset(new BYTE[val]);
		ZeroMemory(m_bits, val);
		av_image_fill_arrays(m_pYUV420->data, m_pYUV420->linesize, m_bits.Ptr(), AV_PIX_FMT_YUV420P, size.cx, size.cy, 1);
		return TRUE;
	}
	BOOL QSVTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&QSVTask::OnMessagePump, this));
	}
	BOOL QSVTask::Close(DWORD dwMS)
	{
		return TinyTaskBase::Close(dwMS);
	}

	void SaveAvFrame(AVFrame *avFrame, FILE* hFile)
	{
		uint32_t pitchY = avFrame->linesize[0];
		uint32_t pitchU = avFrame->linesize[1];
		uint32_t pitchV = avFrame->linesize[2];

		uint8_t *avY = avFrame->data[0];
		uint8_t *avU = avFrame->data[1];
		uint8_t *avV = avFrame->data[2];

		for (int i = 0; i < avFrame->height; i++)
			fwrite(avY + i * pitchY, 1, avFrame->width, hFile);
		//save u  
		for (int i = 0; i < avFrame->height / 2; i++)
			fwrite(avU + i * pitchU, 1, avFrame->width / 2, hFile);
		//save v  
		for (int i = 0; i < avFrame->height / 2; i++)
			fwrite(avV + i * pitchV, 1, avFrame->width / 2, hFile);
	}

	mfxStatus QSVTask::OnInvoke()
	{
		MSDK_FOPEN(m_hFile, "D:\\test1.yuv", "wb");
		MSDK_CHECK_POINTER(m_hFile, MFX_ERR_NULL_PTR);
		mfxStatus status = MFX_ERR_NONE;
		FLV_BLOCK block = { 0 };
		for (;;)
		{
			if (!m_reader.ReadBlock(block))
				break;
			if (block.type == FLV_VIDEO)
			{
				if (block.video.codeID == FLV_CODECID_H264)
				{
					if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
					{
						m_qsv.Open(block.video.data, block.video.size);
					}
					if (block.video.packetType == FLV_NALU)
					{
						Media::SampleTag tag;
						tag.bits = block.video.data;
						tag.size = block.video.size;
						tag.samplePTS = block.pts;
						tag.sampleDTS = block.dts;
						mfxFrameSurface1* ps = NULL;
						m_qsv.Decode(tag, ps);
						if (ps)
						{
							if (m_qsv.Lock(ps))
							{
								SaveAvFrame(m_pYUV420, m_hFile);
								m_qsv.Unlock(ps);
							}
						}
					}
					SAFE_DELETE_ARRAY(block.audio.data);
					SAFE_DELETE_ARRAY(block.video.data);
					Sleep(15);
				}
			}
		}

		MSDK_IGNORE_MFX_STS(status, MFX_ERR_MORE_DATA);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		return status;
	}

	void QSVTask::OnMessagePump()
	{
		OnInvoke();
	}
}