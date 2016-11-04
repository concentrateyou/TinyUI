#pragma once
#include "Utility.h"
#include "Media/TinyWave.h"
#include "SoundPlayer.h"
extern "C"
{
#include "samplerate.h"
}
#pragma comment(lib, "libsamplerate.lib")
using namespace TinyUI::IO;

class Resampler
{
	DISALLOW_COPY_AND_ASSIGN(Resampler);
public:
	Resampler();
	~Resampler();
	BOOL Initialize(const WAVEFORMATEX* pInputFMT, const WAVEFORMATEX* pOutputFMT, Callback<void(FLOAT*, LONG, LPVOID)>& callback);
	BOOL Resample(BYTE* bits, LONG count);
public:
	virtual void OnDataAvailable(FLOAT* bits, LONG size, LPVOID lpParameter);
private:
	const WAVEFORMATEX*		m_pInputFMT;
	const WAVEFORMATEX*		m_pOutputFMT;
	SRC_STATE*				m_resampler;
	DOUBLE					m_resampleRatio;
	TinyScopedArray<FLOAT>	m_bits;
	LONG					m_size;
	DWORD					m_dwBytesPerSample;
	DShow::SoundPlayer		m_player;
	Callback<void(FLOAT*, LONG, LPVOID)> m_callback;
};

