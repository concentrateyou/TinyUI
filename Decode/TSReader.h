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
		BOOL	ReadBlock(TS_BLOCK& block);
	private:
		BOOL	ReadPacket(TS_PACKEG_HEADER& header, TS_PACKET_SECTION*& ps);
		INT		ReadSection(TS_PACKET_SECTION& section, BYTE* bits);
		INT		ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, BYTE* bits);
		INT		ReadPAT(TS_PACKET_PAT* pPAT, TinyArray<TS_PACKET_PROGRAM>& programs, BYTE* bits);
		INT		ReadPTM(TS_PACKET_PMT* pPTM, TinyArray<TS_PACKET_STREAM>& streams, BYTE* bits);
		INT		ReadPES(BYTE* bits);
	private:
		string	ParseDescriptor(BYTE* bits,INT size);
	private:
		BYTE							m_bits[188];
		TinyComPtr<IStream>				m_stream;
		TinyArray<TS_PACKET_STREAM>		m_streams;
		TinyArray<TS_PACKET_PROGRAM>	m_programs;
	};
}


