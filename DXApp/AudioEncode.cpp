#include "stdafx.h"
#include "AudioEncode.h"

AudioEncode::AudioEncode()
	:m_dwOffset(0)
{

}


AudioEncode::~AudioEncode()
{
}

FaacEncode*	AudioEncode::GetEncode()
{
	return &m_aac;
}

AudioCapture* AudioEncode::GetCapture()
{
	return &m_capture;
}

AudioCaptureParam*	AudioEncode::GetParam()
{
	return &m_audioParam;
}

BOOL AudioEncode::Initialize(const AudioCapture::Name& name, const AudioCaptureParam& param)
{

	//ULONG nSampleRate = 44100;  // 采样率
	//UINT nChannels = 2;         // 声道数
	//UINT nPCMBitSize = 16;      // 单样本位数
	//ULONG nInputSamples = 0;
	//ULONG nMaxOutputBytes = 0;

	//int nRet;
	//faacEncHandle hEncoder;
	//faacEncConfigurationPtr pConfiguration;

	//int nBytesRead;
	//int nPCMBufferSize;
	//BYTE* pbPCMBuffer;
	//BYTE* pbAACBuffer;

	//FILE* fpIn; // PCM file for input
	//FILE* fpOut; // AAC file for output

	//fopen_s(&fpIn, "D:\\1235.pcm", "rb");
	//fopen_s(&fpOut, "D:\\out.aac", "wb");

	//// (1) Open FAAC engine
	//hEncoder = faacEncOpen(nSampleRate, nChannels, &nInputSamples, &nMaxOutputBytes);
	//if (hEncoder == NULL)
	//{
	//	printf("[ERROR] Failed to call faacEncOpen()\n");
	//	return -1;
	//}

	//nPCMBufferSize = nInputSamples * nPCMBitSize / 8;
	//pbPCMBuffer = new BYTE[nPCMBufferSize];
	//pbAACBuffer = new BYTE[nMaxOutputBytes];

	//// (2.1) Get current encoding configuration
	//pConfiguration = faacEncGetCurrentConfiguration(hEncoder);
	//pConfiguration->inputFormat = FAAC_INPUT_16BIT;
	//pConfiguration->quantqual = 100;
	//pConfiguration->mpegVersion = MPEG4;
	//pConfiguration->aacObjectType = LOW;
	//pConfiguration->useTns = 1;
	//pConfiguration->useLfe = 0;
	//pConfiguration->bandWidth = 0;
	//pConfiguration->allowMidside = 0;
	//pConfiguration->outputFormat = 1;
	//pConfiguration->shortctl = SHORTCTL_NORMAL;
	//pConfiguration->bitRate = 128 * 1000 / 2;//比特位
	//// (2.2) Set encoding configuration
	//nRet = faacEncSetConfiguration(hEncoder, pConfiguration);

	//for (int i = 0; 1; i++)
	//{
	//	// 读入的实际字节数，最大不会超过nPCMBufferSize，一般只有读到文件尾时才不是这个值
	//	nBytesRead = fread(pbPCMBuffer, 1, nPCMBufferSize, fpIn);

	//	// 输入样本数，用实际读入字节数计算，一般只有读到文件尾时才不是nPCMBufferSize/(nPCMBitSize/8);
	//	nInputSamples = nBytesRead / (nPCMBitSize / 8);

	//	// (3) Encode
	//	nRet = faacEncEncode(
	//		hEncoder, (int*)pbPCMBuffer, nInputSamples, pbAACBuffer, nMaxOutputBytes);

	//	fwrite(pbAACBuffer, 1, nRet, fpOut);

	//	printf("%d: faacEncEncode returns %d\n", i, nRet);

	//	if (nBytesRead <= 0)
	//	{
	//		break;
	//	}
	//}
	//nRet = faacEncClose(hEncoder);

	//delete[] pbPCMBuffer;
	//delete[] pbAACBuffer;
	//fclose(fpIn);
	//fclose(fpOut);


	m_name = name;
	m_audioParam = param;
	m_audioCB = BindCallback(&AudioEncode::OnAudio, this);
	/*BOOL bRes = m_capture.Initialize(m_name, m_audioCB);
	if (!bRes)
		return FALSE;
	bRes = m_capture.Allocate(param);
	if (!bRes)
		return FALSE;*/
	m_wasCB = BindCallback(&AudioEncode::OnDataAvailable, this);
	m_wasCapture.Initialize(m_wasCB);
	return TRUE;
}

BOOL AudioEncode::Open(DWORD dwAudioRate)
{
	WAVEFORMATEX s = m_audioParam.GetFormat();
	BOOL bRes = m_aac.Open(s, (DWORD)dwAudioRate);
	if (!bRes)
		return FALSE;
	bRes = m_wasCapture.Open();
	if (!bRes)
		return FALSE;
	m_resample16CB = BindCallback(&AudioEncode::OnResampleDataAvailable16, this);
	m_resampler.Open(m_wasCapture.GetInputFormat(), &s, m_resample16CB);
	if (!bRes)
		return FALSE;
	bRes = m_wasCapture.Start();
	if (!bRes)
		return FALSE;
	return TRUE;
}

BOOL AudioEncode::Encode()
{
	return m_capture.Start();
}

BOOL AudioEncode::Close()
{
	m_capture.Uninitialize();
	m_aac.Close();
	m_wasCapture.Close();
	m_resampler.Close();
	return TRUE;
}

void AudioEncode::OnResampleDataAvailable16(BYTE* bits, LONG count, LPVOID lpParameter)
{
	count = count / 4;
	if ((m_dwOffset + count * 4) >= 4096)
	{
		DWORD dwL = 4096 - m_dwOffset;
		memcpy(m_buffer + m_dwOffset, bits, dwL);
		m_aac.Encode(m_buffer, 4096, m_dwINC);
		memset(m_buffer, 0, 4096);
		m_dwOffset = count * 4 - dwL;
		memcpy(m_buffer, bits + dwL, m_dwOffset);
	}
	else
	{
		memcpy(m_buffer + m_dwOffset, bits, count * 4);
		m_dwOffset += (count * 4);
	}
}

void AudioEncode::OnDataAvailable(BYTE* bits, LONG count, LPVOID lpParameter)
{
	m_resampler.Resample(bits, count * 8);
}

void AudioEncode::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{
}