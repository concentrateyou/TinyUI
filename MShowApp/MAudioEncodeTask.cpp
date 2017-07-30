#include "stdafx.h"
#include "MAudioEncodeTask.h"
#include "Media/TinyWASAPIAudio.h"
#include "MShowApp.h"

namespace MShow
{
	MAudioEncodeTask::MAudioEncodeTask(MRTMPPusher& pusher)
		:m_pusher(pusher),
		m_aac(BindCallback(&MAudioEncodeTask::OnAAC, this)),
		m_bBreak(FALSE)
	{
	}


	MAudioEncodeTask::~MAudioEncodeTask()
	{
	}
	BOOL MAudioEncodeTask::Submit(INT audioRate)
	{
		vector<DShow::AudioCapture::Name> names;
		if (!DShow::AudioCapture::GetDevices(names))
			return FALSE;
		INT indexName = 0;
		for (indexName = 0;indexName < names.size();indexName++)
		{
			BOOL bIsMA = FALSE;
			TinyWASAPIAudio::IsMicrophoneArray(names[indexName].name(), bIsMA);
			if (bIsMA)
				continue;
			break;
		}
		if (indexName < 0 || indexName >= names.size())
			return FALSE;
		vector<AudioCaptureParam> params;
		if (!DShow::AudioCapture::GetDeviceParams(names[indexName], params))
			return FALSE;
		INT indexParam = 0;
		for (indexParam = 0;indexParam < params.size();indexParam++)
		{
			WAVEFORMATEX waveFMT = params[indexParam].GetFormat();
			if (waveFMT.nSamplesPerSec == 44100 &&
				waveFMT.nChannels == 2 &&
				waveFMT.wBitsPerSample == 16)
			{
				m_waveFMT = waveFMT;
			}
			break;
		}
		if (indexParam < 0 || indexParam >= params.size())
			return FALSE;
		if (!m_audio.Initialize(names[indexName], BindCallback(&MAudioEncodeTask::OnAudio, this)))
			return FALSE;
		if (!m_audio.Allocate(params[indexParam]))
			return FALSE;
		if (!m_aac.Open(m_waveFMT, audioRate))
			return FALSE;
		if (!m_audio.Start())
			return FALSE;
		return TinyTaskBase::Submit(BindCallback(&MAudioEncodeTask::OnMessagePump, this));
	}
	void MAudioEncodeTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_bBreak)
				break;
			Sleep(23);
		}
	}
	BOOL MAudioEncodeTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_aac.Close();
			m_audio.Stop();
			m_audio.Deallocate();
			m_audio.Uninitialize();
			return TRUE;
		}
		return FALSE;
	}
	void MAudioEncodeTask::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
	{
		m_aac.Encode(bits, size);
	}
	void MAudioEncodeTask::OnAAC(BYTE* bits, LONG size, const MediaTag& tag)
	{
		if (tag.INC == 1)
		{
			if (MShowApp::Instance().GetController().GetBaseTime() == -1)
			{
				MShowApp::Instance().GetController().SetBaseTime(timeGetTime());
			}
		}
		Sample sample;
		memcpy(&sample.mediaTag, &tag, sizeof(tag));
		sample.size = size;
		sample.bits = new BYTE[size];
		memcpy(sample.bits, bits, size);
		sample.mediaTag.dwTime = MShowApp::Instance().GetController().GetBaseTime() + tag.INC * sample.mediaTag.PTS;
		m_pusher.Publish(sample);
	}
	INT	MAudioEncodeTask::GetAudioRate() const
	{
		return m_audioRate;
	}
	WAVEFORMATEX MAudioEncodeTask::GetFormat() const
	{
		return m_waveFMT;
	}
	AACEncode&	MAudioEncodeTask::GetAAC()
	{
		return m_aac;
	}
}