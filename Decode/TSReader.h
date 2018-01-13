#pragma once
#include "Common.h"

namespace Decode
{

	class TSParser
	{
	public:
		virtual BOOL Parse(const BYTE* bits, LONG size) = 0;
		GrowableIOBuffer& GetIO();
	private:
		GrowableIOBuffer	m_io;
	};

	class TSH264Parser : public TSParser
	{
	public:
		BOOL Parse(const BYTE* bits, LONG size) OVERRIDE;
	};

	class TSAACParser : public TSParser
	{
	public:
		BOOL Parse(const BYTE* bits, LONG size) OVERRIDE;
	};

	class TS_PACKET_STREAM
	{
	public:
		BYTE StreamType : 8;
		BYTE Reserved1 : 3;
		USHORT ElementaryPID : 13;
		BYTE Reserved2 : 4;
		USHORT ESInfoLength : 12;
	public:
		TS_PACKET_STREAM();
		~TS_PACKET_STREAM();
		TSParser* GetParser();
	private:
		TinyScopedPtr<TSParser>	m_parser;
	public:
		BOOL operator == (const TS_PACKET_STREAM& other);
	};
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
		BOOL	ReadPacket(TS_PACKEG_HEADER& header, TS_BLOCK& block);
		BOOL	ReadPES(TS_PACKET_STREAM* stream, TS_PACKET_PES* pPES, const BYTE* bits, INT offset);
		BOOL	ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, const BYTE* bits);
		BOOL	ReadPAT(TS_PACKET_PAT* pPAT, TinyArray<TS_PACKET_PROGRAM>& programs, const BYTE* bits);
		BOOL	ReadPTM(TS_PACKET_PMT* pPTM, TinyArray<TS_PACKET_STREAM*>& streams, const BYTE* bits);
	private:
		BOOL	ParseAAC(const BYTE* bits, LONG size);
		BOOL	ParseH264(const BYTE* bits, LONG size);
	private:
		BYTE							m_bits[188];
		INT								m_versionNumber;
		INT								m_continuityCounter;
		TinyComPtr<IStream>				m_stream;
		TinyArray<TS_PACKET_STREAM*>	m_streams;
		TinyArray<TS_PACKET_PROGRAM>	m_programs;
	};
}


