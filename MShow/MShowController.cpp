#include "stdafx.h"
#include "MShowController.h"
#include "MPreviewController.h"

namespace MShow
{
	MShowController::MShowController(MShowWindow& window)
		:m_window(window)
	{

	}

	MShowController::~MShowController()
	{
	}

	BOOL MShowController::Initialize()
	{
		m_onRecordClick.Reset(new Delegate<void(void*, INT)>(this, &MShowController::OnRecord));
		m_window.m_btnRecord.EVENT_CLICK += m_onRecordClick;
		m_onPreviewClick.Reset(new Delegate<void(void*, INT)>(this, &MShowController::OnPreview));
		m_window.m_btnPreview.EVENT_CLICK += m_onPreviewClick;
		m_preview.Reset(new MPreviewController(m_window.m_previewView));
		if (!m_preview)
			return FALSE;
		if (!m_preview->Initialize())
			return FALSE;
		if (!m_audioCapture.Initialize())
			return FALSE;
		if (!m_audioAnalyser.Initialize())
			return FALSE;
		m_window.m_txtPreviewURL.SetText("rtmp://live.hkstv.hk.lxdns.com/live/hks");
		m_waveFile.Create("D:\\record.wav", &m_audioCapture.GetFormat());
		return TRUE;
	}

	void MShowController::Uninitialize()
	{
		m_audioCapture.Stop();
		m_audioCapture.Close();
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
		if (m_audioCapture.Open(BindCallback(&MShowController::OnAudio, this)))
		{
			m_audioCapture.Stop();
			m_audioCapture.Start();
		}
	}

	void MShowController::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID)
	{
		TRACE("TS:%f\n", ts);
		m_waveFile.Write(bits, size);
		m_audioAnalyser.Process(m_window.m_analyserBAR.Handle(), bits, size);
	}

	MPreviewController* MShowController::GetPreviewController()
	{
		return m_preview;
	}
}
