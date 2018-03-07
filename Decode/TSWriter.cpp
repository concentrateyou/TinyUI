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

	BOOL TSWriter::WritePAT()
	{
		return TRUE;
	}

	BOOL TSWriter::WritePES(BYTE streamType, USHORT elementaryPID)
	{
		return TRUE;
	}
}
