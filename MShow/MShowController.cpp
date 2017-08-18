#include "stdafx.h"
#include "MShowController.h"
#include "MPreviewController.h"
#include "MShow.h"

namespace MShow
{
	MShowController::MShowController(MShowWindow& window)
		:m_window(window),
		m_previousPTS(0),
		m_bBreak(FALSE)
	{
		m_event.CreateEvent();
	}

	MShowController::~MShowController()
	{
	}

	BOOL MShowController::Initialize()
	{
		m_window.m_txtPreviewURL.SetText("rtmp://live.hkstv.hk.lxdns.com/live/hks");
		m_window.m_address.SetAddress(MAKEIPADDRESS(10, 110, 48, 109));
		m_onRecordClick.Reset(new Delegate<void(void*, INT)>(this, &MShowController::OnRecord));
		m_window.m_btnRecord.EVENT_CLICK += m_onRecordClick;
		m_onPreviewClick.Reset(new Delegate<void(void*, INT)>(this, &MShowController::OnPreview));
		m_window.m_btnPreview.EVENT_CLICK += m_onPreviewClick;
		m_preview.Reset(new MPreviewController(m_window.m_previewView));
		if (!m_preview)
			return FALSE;
		if (!m_preview->Initialize())
		{
			LOG(ERROR) << "Preview Initialize Fail\n";
			return FALSE;
		}
		if (!m_audioCapture.Initialize())
		{
			LOG(ERROR) << "AudioCapture Initialize Fail\n";
			return FALSE;
		}
		if (!m_audioAnalyser.Initialize())
		{
			LOG(ERROR) << "AudioAnalyser Initialize Fail\n";
			return FALSE;
		}
		LOG(INFO) << "MShowController Initialize OK\n";
		return TRUE;
	}

	void MShowController::Uninitialize()
	{
		m_bBreak = TRUE;
		m_task.Close(1000);
		m_audioCapture.Stop();
		m_audioCapture.Close();
		m_audioSDK.Reset(NULL);
		if (m_preview != NULL)
		{
			m_preview->Close();
			m_preview.Reset(NULL);
		}
	}

	void MShowController::OnPreview(void*, INT)
	{
		TinyString szURL = m_window.m_txtPreviewURL.GetText();
		if (m_preview != NULL && !szURL.IsEmpty())
		{
			m_preview->Close();
			m_preview->Open(szURL.CSTR());
		}
	}

	void MShowController::OnRecord(void*, INT)
	{
		DWORD dwAddress = 0;
		m_window.m_address.GetAddress(dwAddress);
		string ip = StringPrintf("%ld.%ld.%ld.%ld", FIRST_IPADDRESS(dwAddress), SECOND_IPADDRESS(dwAddress), THIRD_IPADDRESS(dwAddress), FOURTH_IPADDRESS(dwAddress));
		if (m_task.IsActive())
		{
			m_bBreak = TRUE;
			m_task.Close(1000);
		}
		m_bBreak = FALSE;
		if (m_task.Submit(BindCallback(&MShowController::OnMessagePump, this)))
		{
			m_audioSDK.Reset(new AudioSdk(ip, 6090));
			if (m_audioSDK != NULL)
			{
				if (m_audioSDK->init(44100, 2, 16) == 0)
				{
					if (m_audioCapture.Open(BindCallback(&MShowController::OnAudio, this)))
					{
						m_audioCapture.Stop();
						m_audioCapture.Start();
					}
				}
			}
		}
	}
	void MShowController::OnMessagePump()
	{
		for (;;)
		{
			if (m_bBreak)
				break;
			if (m_event.Lock(1000))
			{
				if (m_audioSDK != NULL)
				{
					AUDIO_SAMPLE sample = { 0 };
					BOOL bRes = m_audioQueue.Pop(sample);
					if (bRes && sample.size > 0)
					{
						m_audioSDK->audio_encode_send(sample.bits + 4, static_cast<INT32>(sample.timestamp));
					}
					m_audioQueue.Free(sample.bits);
				}
			}
		}
	}
	void MShowController::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID)
	{
		if (size == 4096)
		{
			m_audioAnalyser.Process(m_window.m_analyserBAR.Handle(), bits, size);
			LONGLONG currentPTS = MShow::MShowApp::GetInstance().GetCurrentAudioTS() + m_preview->GetBasePTS();
			if (m_previousPTS != currentPTS)
			{
				if ((currentPTS - m_previousPTS) >= 50)
				{
					LOG(ERROR) << "OnAudio: " << currentPTS - m_previousPTS << "Error -----------\n";
				}
				m_previousPTS = currentPTS;
			}
			if (m_audioQueue.GetAllocSize() == 0)
			{
				INT count = 5;
				m_audioQueue.Initialize(count, size + 4);
			}
			AUDIO_SAMPLE sample = { 0 };
			sample.size = size;
			sample.bits = static_cast<BYTE*>(m_audioQueue.Alloc());
			sample.timestamp = currentPTS;
			memcpy(sample.bits + 4, bits, size);
			m_audioQueue.Push(sample);
			m_event.SetEvent();
		}
	}

	MPreviewController* MShowController::GetPreviewController()
	{
		return m_preview;
	}
}
