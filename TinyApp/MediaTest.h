#pragma once
#include "Media/TinyMFMP3Decode.h"
#include "Media/TinyMFMP3Encode.h"
#include "Media/TinyMFAACDecode.h"
#include "Media/TinyMFAACEncode.h"
#include "Media/TinyWave.h"
#include "IO/TinyIO.h"
using namespace TinyUI;
using namespace TinyUI::Media;
using namespace TinyUI::IO;

class MediaTest
{
public:
	MediaTest();
	~MediaTest();

public:
	BOOL WaveToAAC(const string& waveFile, const string& aacFile);
	BOOL AACToWave(const string& aacFile, const string& waveFile);
private:
	void OnAACEncode(BYTE*, LONG, LPVOID);
	void OnAACDecode(BYTE*, LONG, LPVOID);
private:
	TinyMFAACEncode aacencode;
	TinyMFAACDecode aacdecode;
	TinyFile m_file;
	TinyWaveFile m_waveFile;
};

