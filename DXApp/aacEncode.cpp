#include "stdafx.h"
#include "aacEncode.h"


aacEncode::aacEncode()
{
}


aacEncode::~aacEncode()
{
}

BOOL aacEncode::GetSpecificInfo(vector<BYTE>& info)
{
	BYTE* buffer = NULL;
	ULONG size = 0;
	if (!faacEncGetDecoderSpecificInfo(m_aac, &buffer, &size))
	{
		SAFE_FREE(buffer);
		return FALSE;
	}
	info.resize(size);
	memcpy(&info[0], buffer, size);
	SAFE_FREE(buffer);
	return TRUE;
}

BOOL aacEncode::Open(const WAVEFORMATEX& wfx)
{
	m_wfx = wfx;
	m_aac = faacEncOpen(wfx.nSamplesPerSec, wfx.nChannels, &m_inputSamples, &m_maxOutputBytes);
	if (!m_aac)
		return FALSE;
	m_config = faacEncGetCurrentConfiguration(m_aac);
	m_config->outputFormat = 1;
	m_config->version = m_config->outputFormat == 0 ? MPEG4 : MPEG2;
	m_config->inputFormat = FAAC_INPUT_16BIT;
	switch (wfx.wBitsPerSample)
	{
	case 16:
		m_config->inputFormat = FAAC_INPUT_16BIT;
		break;
	case 24:
		m_config->inputFormat = FAAC_INPUT_24BIT;
		break;
	case 32:
		m_config->inputFormat = FAAC_INPUT_32BIT;
		break;
	}
	m_config->aacObjectType = LOW;//��������, LOW:LC����
	m_config->useTns = 0;//˲ʱ��������(temporal noise shaping��TNS)�˲���
	m_config->allowMidside = 1;//M/S����
	m_config->shortctl = SHORTCTL_NORMAL;
	m_config->bandWidth = 0;
	m_config->bitRate = wfx.nAvgBytesPerSec * 8;//����λ
	if (!faacEncSetConfiguration(m_aac, m_config))
		return FALSE;
	m_output.resize(m_maxOutputBytes);
	return TRUE;
}
BOOL aacEncode::Encode(BYTE* bits, LONG size, EncoderPacket& packet)
{
	ZeroMemory(&packet, sizeof(packet));
	packet.type = ENCODER_AUDIO;
	if (!bits)
		return FALSE;
	packet.size = faacEncEncode(m_aac, (int32_t*)bits, m_inputSamples, &m_output[0], m_maxOutputBytes);
	if (packet.size > 0)
	{
		packet.bits = new BYTE[packet.size];
		memcpy(packet.bits, &m_output[0], packet.size);
		packet.timestamp = m_timestamp;
		packet.duration = (1024 * 1000) / m_wfx.nSamplesPerSec;
		return TRUE;
	}
	return FALSE;
}
void aacEncode::Close()
{
	if (m_aac)
	{
		faacEncClose(m_aac);
		m_aac = NULL;
	}
}
