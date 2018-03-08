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

	BOOL TSWriter::WritePAT(BYTE sPID, BOOL bPL, BOOL bPCR)
	{
		TinyBitWriter writer;
		writer.Initialize(m_bits, TS_PACKET_SIZE);
		writer.WriteBits(8, TS_SYNC_BYTE);
		writer.WriteBits(1, 0);
		writer.WriteBits(1, bPL ? 1 : 0);
		writer.WriteBits(1, 0);
		writer.WriteBits(13, sPID);
		//writer.WriteBits(2, );
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
