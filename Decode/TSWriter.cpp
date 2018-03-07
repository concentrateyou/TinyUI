#include "stdafx.h"
#include "TSWriter.h"

namespace Decode
{
	TSWriter::TSWriter()
	{
	}

	TSWriter::~TSWriter()
	{
	}

	BOOL TSWriter::Create(LPCSTR pzFile)
	{
		HRESULT hRes = SHCreateStreamOnFile(pzFile, STGM_CREATE | STGM_WRITE, &m_stream);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}

	BOOL TSWriter::WriteAAC()
	{
		return TRUE;
	}
	BOOL TSWriter::WriteH264()
	{
		return TRUE;
	}

	BOOL TSWriter::WritePAT(BYTE sPID)
	{
		BYTE header[4];
		header[0] = TS_SYNC_BYTE;
		header[1] = 0x01;
		header[2] = sPID & 0xFF;
		return TRUE;
	}

	BOOL TSWriter::WritePMT()
	{
		return TRUE;
	}

	BOOL TSWriter::WritePES(BYTE streamType, USHORT elementaryPID)
	{
		return TRUE;
	}
}
