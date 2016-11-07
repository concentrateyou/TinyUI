#pragma once
#include "Utility.h"
#include "Media/TinyWave.h"
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
	BOOL Open(const WAVEFORMATEX* pInputFMT, const WAVEFORMATEX* pOutputFMT, Callback<void(FLOAT*, LONG, LPVOID)>& callback);
	BOOL Resample(BYTE* bits, LONG count);
	BOOL Close();
public:
	virtual void OnDataAvailable(FLOAT* bits, LONG size, LPVOID lpParameter);
private:
	const WAVEFORMATEX*		m_pFMTI;
	const WAVEFORMATEX*		m_pFMTO;
	SRC_STATE*				m_resampler;
	DOUBLE					m_resampleRatio;
	TinyScopedArray<FLOAT>	m_bits;
	LONG					m_size;
	DWORD					m_dwBytesPerSample;
	Callback<void(FLOAT*, LONG, LPVOID)> m_callback;
};

