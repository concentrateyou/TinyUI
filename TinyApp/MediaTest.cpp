#include "stdafx.h"
#include "MediaTest.h"


MediaTest::MediaTest()
	:m_dwhnsSampleDuration(0)
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
	WAVEFORMATEX pcmFMT = *wave.GetFormat();
	WAVEFORMATEX aacFMT = pcmFMT;
	aacFMT.nAvgBytesPerSec = 192 * 1000; //BitRate * 1000 /8;
	if (!aacencode.Open(&aacFMT, BindCallback(&MediaTest::OnAACEncode, this)))
		return FALSE;

	m_aacFile.Create(aacFile.c_str());

	TinyComPtr<IMFMediaType> inputType;
	aacencode.GetInputType(&inputType);

	TinyComPtr<IMFMediaType> outputType;
	aacencode.GetOutputType(&outputType);

	BYTE data[1024 * 4];
	LONG dwNumberOfBytesRead = 0;
	LONGLONG hnsSampleTime = 0;
	do
	{
		wave.Read(data, 1024 * 4, &dwNumberOfBytesRead);
		FLOAT samples = dwNumberOfBytesRead / pcmFMT.nChannels / pcmFMT.wBitsPerSample / 8;
		LONGLONG hnsSampleDuration = static_cast<LONGLONG>((samples * (LONGLONG)10000000) / pcmFMT.nSamplesPerSec);
		if (aacencode.Encode(data, dwNumberOfBytesRead, hnsSampleTime, hnsSampleDuration))
		{
			hnsSampleTime += hnsSampleDuration;
		}
	} while (dwNumberOfBytesRead > 0);
	aacencode.Close();

	return TRUE;
}

BOOL MediaTest::AACToWave(const string& aacFile, const string& waveFile)
{
	m_aacFile.Open(aacFile.c_str());

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
	/*if (!aacdecode.Open(&waveFormat, 192000,TRUE, BindCallback(&MediaTest::OnAACDecode, this)))
		return FALSE;*/

	BYTE data[1024 * 4];
	LONG dwNumberOfBytesRead = 0;
	do
	{
		dwNumberOfBytesRead = m_aacFile.Read(data, 1024 * 4);
		SampleTag tag = { 0 };
		/*if (!aacdecode.Decode(data, dwNumberOfBytesRead))
		{
			INT a = 0;
		}*/
	} while (dwNumberOfBytesRead > 0);

	aacdecode.Close();
	return TRUE;
}

BOOL MediaTest::WaveToMP3(const string& waveFile, const string& mp3File)
{
	TinyWaveFile wave;
	if (!wave.Open((LPTSTR)&waveFile[0]))
		return FALSE;
	return TRUE;
}
BOOL MediaTest::MP3ToWave(const string& mp3File, const string& waveFile)
{
	return FALSE;
}

void MediaTest::OnAACEncode(BYTE* bits, LONG size, LPVOID)
{
	m_aacFile.Write(bits, size);
}

void MediaTest::OnAACDecode(BYTE* bits, LONG size, LPVOID)
{
	m_waveFile.Write(bits, size);
}
