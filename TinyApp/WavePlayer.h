#pragma once
#include "IO/TinyWorker.h"
#include "Media/TinyWave.h"
#include "Media/TinySoundPlayer.h"
#include "AudioAnalyser.h"
using namespace TinyFramework;
using namespace TinyFramework::Media;
using namespace TinyFramework::IO;

class WavePlayer : public TinyWorker
{
public:
	WavePlayer();
	~WavePlayer();
public:
	BOOL Submit(HWND hWND);
	BOOL Close(DWORD dwMS) OVERRIDE;
private: 
	void OnMessagePump();
public:
	HWND			m_hWND;
	TinyEvent		m_events[3];
	TinySoundPlayer m_player;
	TinyWaveFile	m_waveFile;
	/*Decode::AudioAnalyser	m_analyser1;*/
	Decode::AudioAnalyser	m_analyser;
};

