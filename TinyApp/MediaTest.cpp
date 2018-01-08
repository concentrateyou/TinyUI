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
	TinyMFAACEncode aacencode;
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
	TinyMFAACDecode aacdecode;
	if (!aacdecode.Open(&waveFormat, 192000, TRUE))
		return FALSE;
	BYTE data[1024 * 4];
	LONG dwNumberOfBytesRead = 0;
	do
	{
		dwNumberOfBytesRead = m_aacFile.Read(data, 1024 * 4);
		SampleTag tag = { 0 };
		BYTE* bo = NULL;
		DWORD so = 0;
		if (!aacdecode.Decode(tag, bo, so))
		{
			INT a = 0;
		}
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
BOOL MediaTest::MP3ToWave(const string& szFileMp3, const string& szFileWave)
{
	TinyMFMP3Decode mp3decode;
	TinyMP3File mp3File;
	if (!mp3File.Open(szFileMp3.c_str()))
		return FALSE;
	MPEGLAYER3WAVEFORMAT* mp3FMT = mp3File.GetFormat();
	WAVEFORMATEX waveFMT;
	waveFMT.wFormatTag = WAVE_FORMAT_PCM;
	waveFMT.cbSize = 0;
	waveFMT.nChannels = 2;
	waveFMT.nSamplesPerSec = 44100;
	waveFMT.wBitsPerSample = 16;
	waveFMT.nBlockAlign = 4;
	waveFMT.nAvgBytesPerSec = waveFMT.nSamplesPerSec * waveFMT.nBlockAlign;
	TinyWaveFile waveFile;
	if (!waveFile.Create((LPTSTR)szFileWave.c_str(), &waveFMT))
		return FALSE;
	if (!mp3decode.Open(mp3FMT, &waveFMT))
		return FALSE;
	LONG dwNumberOfBytesRead = 0;
	LONGLONG time = 0;
	do
	{
		SampleTag tag = { 0 };
		if (!mp3File.Read(tag.bits, (LPLONG)&dwNumberOfBytesRead, time))
			return FALSE;
		tag.size = dwNumberOfBytesRead;
		BYTE* bo = NULL;
		DWORD so = 0;
		if (mp3decode.Decode(tag, bo, so))
		{
			waveFile.Write(bo, so);
		}
	} while (dwNumberOfBytesRead > 0);
	return FALSE;
}

BOOL MediaTest::H264ToI420(const string& h264File, const string& i420File)
{
	TinyFile sFile;
	if (!sFile.Create(i420File.c_str()))
		return FALSE;
	Decode::H264Reader reader;
	reader.Open(h264File.c_str());
	TinyMFIntelQSVDecode qsvdecoder;
	if (!qsvdecoder.Open({ 1280,720 }, 25))
		return FALSE;
	//NALU nalu = { 0 };
	//do
	//{
	//	reader.ReadNALU(nalu);
	//	TRACE("Size:%d\n", nalu.size);
	///*	SampleTag tag = { 0 };
	//	tag.bits = nalu.bits;
	//	tag.size = nalu.size;
	//	BYTE* bo = NULL;
	//	DWORD so = 0;
	//	if (qsvdecoder.Decode(tag, bo, so))
	//	{
	//		sFile.Write(bo, so);
	//	}*/
	//} while (nalu.size > 0);
	//qsvdecoder.Close();
	return TRUE;
}

void MediaTest::OnAACEncode(BYTE* bits, LONG size, LPVOID)
{
	m_aacFile.Write(bits, size);
}

void MediaTest::OnAACDecode(BYTE* bits, LONG size, LPVOID)
{
	m_waveFile.Write(bits, size);
}
