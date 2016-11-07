#include "stdafx.h"
#include "NoiseSuppression.h"

NoiseSuppression::NoiseSuppression()
	:m_pNS(NULL)
{
}


NoiseSuppression::~NoiseSuppression()
{
}


BOOL NoiseSuppression::Open(const WAVEFORMATEX* pFMT)
{
	if (WebRtcNs_Create(&m_pNS) != 0)
		return FALSE;
	if (WebRtcNs_Init(m_pNS, 16000) != 0)
		return FALSE;
	if (WebRtcNs_set_policy(m_pNS, 2) != 0)
		return FALSE;
	m_waveFMT = *pFMT;
	m_bits.Reset(new BYTE[320 * pFMT->nBlockAlign]);
	return TRUE;
}

BOOL NoiseSuppression::Denoise(BYTE* bits, LONG count)
{
	SHORT* bitsI = (SHORT*)bits;
	INT size = (count * m_waveFMT.nBlockAlign) / sizeof(SHORT);
	/*TinyScopedArray<SHORT>	bitsO(new SHORT[size]);
	WebRtcNs_Process(m_pNS, bitsI, NULL, bitsO, NULL);*/
	return TRUE;
}

BOOL NoiseSuppression::Close()
{
	if (m_pNS != NULL)
	{
		if (WebRtcNs_Free(m_pNS) == 0)
		{
			m_pNS = NULL;
			return TRUE;
		}
	}
	return FALSE;
}