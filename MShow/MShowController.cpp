#include "stdafx.h"
#include "MShowController.h"
#include "MPreviewController.h"
#include "MShow.h"

namespace MShow
{
	MShowController::MShowController(MShowWindow& window)
		:m_window(window),
		m_previousPTS(0),
		m_bBreak(FALSE),
		m_bError(FALSE)
	{
		m_event.CreateEvent();
	}

	MShowController::~MShowController()
	{

	}

	BOOL MShowController::Initialize()
	{
		m_window.m_txtPreviewURL.SetText("http://10.110.48.109:52193/6696864323");
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
			LOG(ERROR) << "Preview Initialize Fail" << endl;
			return FALSE;
		}
		if (!m_audioDSP.Initialize(BindCallback(&MShowController::OnAudioDSP, this)))
		{
			LOG(ERROR) << "AudioDSP Initialize Fail" << endl;
			return FALSE;
		}
		if (!m_audioAnalyser.Initialize())
		{
			LOG(ERROR) << "AudioAnalyser Initialize Fail" << endl;
			return FALSE;
		}
		LOG(INFO) << "MShowController Initialize OK" << endl;
		return TRUE;
	}

	void MShowController::Uninitialize()
	{
		m_bBreak = TRUE;
		m_task.Close(1000);
		m_audioDSP.Stop();
		m_audioDSP.Close();
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
			if (m_preview->Open(szURL.CSTR()))
			{
				LOG(INFO) << "Preview Open OK" << endl;
			}
			else
			{
				LOG(ERROR) << "Preview Open Fail" << endl;
			}
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
					LOG(INFO) << "AudioSDK init OK" << endl;
					m_audioDSP.Close();
					if (m_audioDSP.Open())
					{
						m_audioDSP.Stop();
						m_audioDSP.Start();
					}
				}
				else
				{
					LOG(ERROR) << "AudioSDK init Fail" << endl;
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

	void MShowController::OnAudioDSP(BYTE* bits, LONG size)
	{
		if (size == 4096)
		{
			m_audioAnalyser.Process(m_window.m_analyserBAR.Handle(), bits, size);
			LONGLONG currentPTS = MShow::MShowApp::GetInstance().GetCurrentAudioTS() + m_preview->GetBasePTS();
			if (m_previousPTS != currentPTS)
			{
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
