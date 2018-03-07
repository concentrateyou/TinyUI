#pragma once
#include "Common.h"
#include <vector>

namespace Decode
{
	/// <summary>
	/// TS文件包装
	/// </summary>
	class TSWriter
	{
		DISALLOW_COPY_AND_ASSIGN(TSWriter)
	public:
		TSWriter();
		virtual ~TSWriter();
		BOOL Create(LPCSTR pzFile);
	private:
		BOOL WriteAAC();
		BOOL WriteH264();
		BOOL WritePAT(BYTE sPID);
		BOOL WritePMT();
		BOOL WritePES(BYTE streamType, USHORT elementaryPID);
	private:
		TinyComPtr<IStream>		m_stream;
	};
}


