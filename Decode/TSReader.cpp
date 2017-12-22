#include "stdafx.h"
#include "TSReader.h"

namespace Decode
{
	TSReader::TSReader()
	{
	}

	TSReader::~TSReader()
	{
	}
	BOOL TSReader::OpenFile(LPCSTR pzFile)
	{
		if (!pzFile)
			return FALSE;
		this->Close();
		HRESULT hRes = SHCreateStreamOnFileA(pzFile, STGM_READ | STGM_FAILIFTHERE, &m_stream);
		if (hRes != S_OK)
			return FALSE;
		BYTE data[4];
		ULONG ls = 0;
		hRes = m_stream->Read(data, 4, &ls);
		if (hRes != S_OK || ls != 4)
			return FALSE;
		TS_PACKEG_HEADER header = { 0 };
		header.Syncbyte = data[0];
		header.TransportErrorIndicator = data[1] & 0x80;
		header.PayloadUnitStartIndicator = data[1] & 0x40;
		header.TransportPriority = data[1] & 0x20;
		header.PID = (data[1] & 0x1F) << 8 | data[2];
		header.TransportScramblingControl = data[3] & 0xC0;
		header.AdaptationFieldControl = data[3] & 0x30;
		header.ContinuityCounter = data[3] & 0x0F;
		if (header.Syncbyte != 0x47)
			return FALSE;

		return TRUE;
	}
	BOOL TSReader::Close()
	{
		m_stream.Release();
		return TRUE;
	}
}

