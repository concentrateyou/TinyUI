#pragma once
#include "MShowWindow.h"
#include "MAudioCapture.h"
#include "AudioAnalyser.h"
using namespace DShow;
using namespace Decode;

namespace MShow
{
	/// <summary>
	/// Ö÷¿ØÖÆÆ÷
	/// </summary>
	class MShowController
	{
		DISALLOW_COPY_AND_ASSIGN(MShowController)
	public:
		MShowController(MShowWindow& window);
		~MShowController();
		BOOL	Initialize();
		void	Uninitialize();
		MPreviewController* GetPreviewController();
	private:
		void OnPreview(void*, INT);
		void OnRecord(void*, INT);
		void OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID);
	private:
		TinyLock		m_lock;
		MShowWindow&	m_window;
		MAudioCapture	m_audioCapture;
		AudioAnalyser	m_audioAnalyser;
		TinyWaveFile	m_waveFile;
		TinyScopedPtr<MPreviewController>			m_preview;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onPreviewClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onRecordClick;
	};
}


