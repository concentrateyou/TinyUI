#include "stdafx.h"
#include "MediaTest.h"


MediaTest::MediaTest()
{
}


MediaTest::~MediaTest()
{
}

BOOL MediaTest::WaveToAAC(const string& waveFile, const string& aacFile)
{
	TinyWaveFile wave;
	if (!wave.Open((LPTSTR)&waveFile[0]))
		return FALSE;
	WAVEFORMATEX aacFormat = *wave.GetFormat();
	aacFormat.nAvgBytesPerSec = 192 * 1000; //BitRate * 1000 /8;
	if (!aacencode.Open(&aacFormat, BindCallback(&MediaTest::OnAACEncode, this)))
		return FALSE;
	m_file.Create(aacFile.c_str());
	BYTE data[1024 * 8];
	LONG dwNumberOfBytesRead = 0;
	do
	{
		wave.Read(data, 1024 * 8, &dwNumberOfBytesRead);
		LONGLONG hnsSampleTime = timeGetTime();
		LONGLONG hnsSampleDuration = (aacFormat.nChannels * (LONGLONG)10000000) / aacFormat.nSamplesPerSec;
		aacencode.Encode(data, dwNumberOfBytesRead, hnsSampleTime, hnsSampleDuration);
	} while (dwNumberOfBytesRead > 0);
	return TRUE;
}

BOOL MediaTest::AACToWave(const string& aacFile, const string& waveFile)
{
	m_file.Open(aacFile.c_str());

	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.cbSize = 0;
	waveFormat.nChannels = 2;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nBlockAlign = 4;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	if (!m_waveFile.Create((LPTSTR)&waveFile[0], &waveFormat))
		return FALSE;
	if (!aacdecode.Open(&waveFormat, 192000, FALSE, BindCallback(&MediaTest::OnAACDecode, this)))
		return FALSE;

	BYTE data[1024];
	LONG dwNumberOfBytesRead = 0;
	do
	{
		dwNumberOfBytesRead = m_file.Read(data, 1024);
		aacdecode.Decode(data, dwNumberOfBytesRead);
	} while (dwNumberOfBytesRead > 0);

	return TRUE;
}

void MediaTest::OnAACEncode(BYTE* bits, LONG size, LPVOID)
{
	m_file.Write(bits, size);
}

void MediaTest::OnAACDecode(BYTE* bits, LONG size, LPVOID)
{
	m_waveFile.Write(bits, size);
}
