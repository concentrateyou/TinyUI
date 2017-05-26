#include "stdafx.h"
#include "amf.h"
#include "FLVWriter.h"
#include "RTMPStream.h"

namespace Decode
{
	FLVWriter::FLVWriter()
	{

	}
	FLVWriter::~FLVWriter()
	{

	}
	BOOL FLVWriter::Create(LPCSTR pzFile)
	{
		HRESULT hRes = SHCreateStreamOnFileA(pzFile, STGM_CREATE | STGM_FAILIFTHERE, &m_stream);
		if (hRes != S_OK)
			return FALSE;

		return TRUE;
	}
	BOOL FLVWriter::Close()
	{
		return TRUE;
	}
}

