#pragma once
#include "Media/TinyWave.h"
#include "Media/TinyMFMP3Decode.h"
#include "Media/TinyMFMP3Encode.h"
#include "Media/TinyMFAACDecode.h"
#include "Media/TinyMFAACEncode.h"
#include "Media/TinyMFIntelQSVDecode.h"
#include "Media/TinyMP3File.h"
#include "Media/TinyMP4File.h"
#include "H264Reader.h"
#include "IO/TinyIO.h"
using namespace TinyUI;
using namespace TinyUI::Media;
using namespace TinyUI::IO;
using namespace Decode;

class MediaTest
{
public:
	MediaTest();
	~MediaTest();

public:
	BOOL WaveToAAC(const string& waveFile, const string& aacFile);
	BOOL AACToWave(const string& aacFile, const string& waveFile);

	BOOL WaveToMP3(const string& waveFile, const string& mp3File);
	BOOL MP3ToWave(const string& mp3File, const string& waveFile);

	BOOL H264ToI420(const string& h264File, const string& i420File);

private:
	void OnAACEncode(BYTE*, LONG, LPVOID);
	void OnAACDecode(BYTE*, LONG, LPVOID);
private:
	
	TinyFile m_aacFile;
	TinyWaveFile m_waveFile;
	DWORD m_dwhnsSampleDuration;
};

