#pragma once
#include "Common.h"

namespace Decode
{
	/// <summary>
	/// TSÎÄ¼þ½âÎö
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
		INT		ReadHeader(TS_PACKEG_HEADER& s, BYTE* data);
		INT		ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, BYTE* data);
		INT		ReadPAT(TS_PACKEG_PAT& myPAT, TinyArray<TS_PACKET_PROGRAM>& programs, BYTE* data);
	private:
		BYTE							m_bits[188];
		TinyComPtr<IStream>				m_stream;
		TinyArray<TS_PACKET_PROGRAM>	m_programs;
	};
}


