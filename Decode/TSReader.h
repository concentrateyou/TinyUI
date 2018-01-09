#pragma once
#include "Common.h"

namespace Decode
{
	/// <summary>
	/// TS�ļ�����
	/// </summary>
	class TSReader
	{
		DISALLOW_COPY_AND_ASSIGN(TSReader)
	public:
		TSReader();
		virtual ~TSReader();
		BOOL	OpenFile(LPCSTR pzFile);
		BOOL	Close();
		BOOL	ReadPacket();
	private:
		INT		ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, BYTE* bits);
		INT		ReadPAT(TS_PACKEG_PAT& myPAT, TinyArray<TS_PACKET_PROGRAM>& programs, BYTE* bits);
	private:
		INT						m_offset;
		BYTE					m_bits[188];
		TinyComPtr<IStream>		m_stream;
	};
}


