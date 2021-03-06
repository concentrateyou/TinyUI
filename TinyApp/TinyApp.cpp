// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TinyApp.h"
#include "MainFrame.h"
#include "ChatFrame.h"
#include "ChatDlg.h"
#include "MFFrame.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Render/TinyDDraw.h"
#include "Media/TinyWASAPIAudioCapture.h"
#include "Media/TinyWASAPIAudioRender.h"
#include "Media/TinyAudioDSPCapture.h"
#include "Network/TinyIOServer.h"
#include "Network/TinySocket.h"
#include "Media/TinyWave.h"
#include "Common/TinyHook.h"
#include "Media/TinyMFMP3Decode.h"
#include "MPG123Decode.h"
#include "FLVParser.h"
#include "Media/TinyMP3File.h"
#include "Network/TinyURL.h"
#include "Network/TinyDNS.h"
#include "Common/TinySignal.h"
#include "SkinWindow.h"
#include "Media/TinyWave.h"
#include "FLVReader.h"
#include "TSReader.h"
#include "Media/TinySoundCapture.h"
#include "MediaTest.h"
#include "Media/TinyMFIntelQSVDecode.h"
#include <fstream>
#include "IO/TinyBitWriter.h"
#include "Network/TinyPing.h"
#include "HLSReader.h"
#include "GLView.h"

#include <unordered_map>
using namespace std;

#include "IO/TinyProcess.h"

using namespace TinyFramework;
using namespace TinyFramework::Network;
using namespace TinyFramework::Media;
using namespace Decode;

BOOL LoadSeDebugPrivilege()
{
	DWORD   err;
	HANDLE  hToken;
	LUID    Val;
	TOKEN_PRIVILEGES tp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		err = GetLastError();
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Val))
	{
		err = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = Val;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
	{
		err = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}
	CloseHandle(hToken);
	return TRUE;
}

class TSDecoder
{
public:
	TSDecoder();
	virtual ~TSDecoder();
	BOOL Open(LPCSTR pzFile);
	void Invoke();
	void Close();
	void OnConfigChange(const BYTE* bits, LONG size, BYTE streamType, LPVOID);
private:
	INT				m_count;
	x264Decoder		m_x264;
	AACDecoder		m_aac;
	TSReader		m_reader;
	TinyWaveFile	m_waveFile;
	FILE*			m_h264File;
	FILE*			m_aacFile;
};
TSDecoder::TSDecoder()
	:m_count(0),
	m_aacFile(NULL),
	m_h264File(NULL)
{

}
TSDecoder::~TSDecoder()
{

}
BOOL TSDecoder::Open(LPCSTR pzFile)
{
	m_reader.Close();
	m_reader.SetConfigCallback(BindCallback(&TSDecoder::OnConfigChange, this));
	m_reader.OpenFile(pzFile);
	return TRUE;
}

void TSDecoder::Close()
{
	if (m_aacFile != NULL)
	{
		fclose(m_aacFile);
		m_aacFile = NULL;
	}

	if (m_h264File != NULL)
	{
		fclose(m_h264File);
		m_h264File = NULL;
	}

	m_waveFile.Close();
	m_reader.Close();
}

void TSDecoder::OnConfigChange(const BYTE* bits, LONG size, BYTE streamType, LPVOID)
{
	if (streamType == TS_STREAM_TYPE_VIDEO_H264)
	{
		fopen_s(&m_h264File, "D:\\test111.h264", "wb+");
		m_x264.Close();
		m_x264.Initialize({ 1280,720 }, { 1280,720 });
		m_x264.Open(const_cast<BYTE*>(bits), size);
		fwrite(bits, sizeof(BYTE), size, m_h264File);
	}
	if (streamType == TS_STREAM_TYPE_AUDIO_AAC)
	{
		/*	m_aac.Close();
			m_aac.Open(const_cast<BYTE*>(bits), size, 16);
			m_waveFile.Create("D:\\test111.wav", m_aac.GetFormat());*/
	}
}

INT videos = 0;
INT audios = 0;

void TSDecoder::Invoke()
{
	TS_BLOCK block = { 0 };
	for (;;)
	{
		ZeroMemory(&block, sizeof(block));
		if (!m_reader.ReadBlock(block))
		{
			SAFE_DELETE_ARRAY(block.audio.data);
			SAFE_DELETE_ARRAY(block.video.data);
			break;
		}
		if (block.streamType == TS_STREAM_TYPE_VIDEO_H264)
		{
			fwrite(block.video.data, sizeof(BYTE), block.video.size, m_h264File);
			TRACE("H264 Size:%d\n", block.video.size);
			//SampleTag tag = { 0 };
			//tag.size = block.video.size;
			//tag.bits = block.video.data;
			//tag.sampleDTS = block.dts;
			//tag.samplePTS = block.pts;
			////BYTE* bo = NULL;
			////LONG so = 0;
			////INT iRes = m_x264.Decode(tag, bo, so);
			////if (iRes == 0)
			////{
			////	/*tag.sampleDTS = tag.samplePTS = m_x264.GetYUV420()->pts;
			////	BITMAPINFOHEADER bi = { 0 };
			////	bi.biSize = sizeof(BITMAPINFOHEADER);
			////	bi.biWidth = 1280;
			////	bi.biHeight = -720;
			////	bi.biPlanes = 1;
			////	bi.biBitCount = 32;
			////	bi.biCompression = BI_RGB;*/
			////	//SaveBitmap(bi, bo, so, StringPrintf("D:\\test\\%s.bmp", GenerateGUID().c_str()).c_str());
			////}
			++videos;
		}
		/*if (block.streamType == TS_STREAM_TYPE_AUDIO_AAC)
		{
			SampleTag tag = { 0 };
			tag.size = block.audio.size;
			tag.bits = block.audio.data;
			tag.sampleDTS = block.dts;
			tag.samplePTS = block.pts;
			BYTE* bo = NULL;
			LONG so = 0;
			if (m_aac.Decode(tag, bo, so))
			{
				m_waveFile.Write(bo, so);
			}
			++audios;
		}*/
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}
}
BOOL GetFileName(std::string& value)
{
	HKEY hKey = NULL;
	LSTATUS status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\2c437e0f-7139-58bf-a692-457cda9449d1", 0, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, &hKey);
	if (status != ERROR_SUCCESS)
		return FALSE;
	CHAR sz[MAX_PATH];
	DWORD size = MAX_PATH;
	status = RegQueryValueEx(hKey, "InstallLocation", NULL, NULL, (LPBYTE)sz, &size);
	if (status != ERROR_SUCCESS)
		goto _ERROR;
	value.append(sz);
	value.append("\\");
	status = RegQueryValueEx(hKey, "ShortcutName", NULL, NULL, (LPBYTE)sz, &size);
	if (status != ERROR_SUCCESS)
		goto _ERROR;
	value.append(sz);
	value.append(".exe");
_ERROR:
	RegCloseKey(hKey);
	return status == ERROR_SUCCESS;
}


INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WSADATA   wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	MFStartup(MF_VERSION);

	HRESULT hRes = OleInitialize(NULL);
	LoadSeDebugPrivilege();
	CoInitialize(NULL);
	avcodec_register_all();

	TinyStringMap<INT> maps;

	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	SkinWindow view;
	view.Create(NULL, "");
	view.UpdateWindow();
	INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();

	CoUninitialize();
	OleUninitialize();
	MFShutdown();
	WSACleanup();

	return loopRes;

};

//HLSReader reader;
//reader.Open("http://live.hkstv.hk.lxdns.com/live/hks/playlist.m3u8");
//for (;;)
//{
//	reader.ReadSegments();
//	Sleep(1000);
//}
//TSDecoder decoder;
//decoder.Open("D:\\1.ts");
//decoder.Invoke();
//decoder.Close();
////FILE* hFile1 = NULL;
//fopen_s(&hFile1, "D:\\test.264", "wb+");
////FILE* hFile2 = NULL;
//fopen_s(&hFile2, "D:\\test.aac", "wb+");
//FLVReader reader;
//reader.OpenFile("D:\\10s.flv");
//for (;;)
//{
//	FLV_BLOCK block = { 0 };
//	BOOL bRes = reader.ReadBlock(block);
//	if (block.type == FLV_AUDIO)
//	{
//		if (block.audio.codeID == FLV_CODECID_AAC)
//		{
//			if (block.audio.packetType == FLV_AudioSpecificConfig)
//			{
//				/*TinyBitReader reader;
//				reader.Initialize(block.audio.data, block.audio.size);
//				INT audioObjectType = 0;
//				reader.ReadBits(5, &audioObjectType);
//				INT samplingFrequencyIndex = 0;
//				reader.ReadBits(4, &samplingFrequencyIndex);
//				INT channelConfiguration = 0;
//				reader.ReadBits(4, &channelConfiguration);
//				INT frameLengthFlag = 0;
//				reader.ReadBits(1, &frameLengthFlag);
//				INT dependsOnCoreCoder = 0;
//				reader.ReadBits(1, &dependsOnCoreCoder);
//				INT extensionFlag = 0;
//				reader.ReadBits(1, &extensionFlag);
//				INT a = 0;*/
//			}
//		}
//	}
//	SAFE_DELETE_ARRAY(block.audio.data);
//	SAFE_DELETE_ARRAY(block.video.data);
//	if (!bRes)
//		break;
//}
//reader.Close();
//reader.OpenFile("D:\\1.ts");
//for (;;)
//{
//	TS_BLOCK block;
//	if (!reader.ReadBlock(block))
//	{
//		break;
//	}
//	if (block.streamType == TS_STREAM_TYPE_VIDEO_H264)
//	{
//		++count;
//		TRACE("Size:%d\n", block.video.size);
//	}
//	SAFE_DELETE_ARRAY(block.audio.data);
//	SAFE_DELETE_ARRAY(block.video.data);
//}
////fclose(hFile1);
////fclose(hFile2);
//MediaTest test;
////test.H264ToI420("D:\\Media\\test.264", "D:\\Media\\test.yuv");
////test.MP3ToWave("D:\\Media\\李玉刚-刚好遇见你.mp3", "D:\\Media\\李玉刚-刚好遇见你.wav");
//FILE* hFile = NULL;
//fopen_s(&hFile, "D:\\test.264", "rb");
//TinyMFIntelQSVDecode decoder;
//decoder.Open({ 1280,720 }, 25);
//INT pos = 0;
//TinyScopedArray<BYTE> bits(new BYTE[1024 * 64]);
//for (;;)
//{
//	size_t count = fread(bits, 1, 1024 * 64, hFile);
//	if (count == 0)
//		break;
//	BYTE* bo = NULL;
//	DWORD so = 0;
//	SampleTag tag = { 0 };
//	tag.bits = bits;
//	tag.size = count;
//	decoder.Decode(tag, bo, so);
//}
////fclose(hFile);
//TinyVisualResource::GetInstance().Load("skin\\resource.xml");
//::DefWindowProc(NULL, 0, 0, 0L);
//TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
//TinyMessageLoop theLoop;
//TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
//TinyVisualLayeredWindow uiImpl;
//string szFile = StringPrintf("%s\%s", TinyVisualResource::GetInstance().GetDefaultPath().c_str(), "skin\\contextmenu.xml");
//uiImpl.Create(NULL, szFile.c_str());
//TinyVisualContextMenu* contextmenu = (TinyVisualContextMenu*)uiImpl.GetDocument().GetVisualByName("menu1");
//contextmenu->Add("Test1");
//contextmenu->Add("Test2");
//contextmenu->Add("Test3");
//uiImpl.Update();
//INT loopRes = theLoop.MessageLoop();
//TinyApplication::GetInstance()->RemoveMessageLoop();
//TinyApplication::GetInstance()->Uninitialize();