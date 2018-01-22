#pragma once
#include "Common.h"
#include "H264Parser.h"

namespace Decode
{
	class TSParser
	{
	public:
		TSParser();
		virtual ~TSParser();
		virtual BYTE GetStreamType() const = 0;
		virtual BOOL Parse(TS_BLOCK& block) = 0;
		void	SetCapacity(INT capacity);
		void	Add(BYTE* bits, INT size);
		void	Reset();
		BYTE*	data();
		INT		size() const;
	private:
		INT						m_capacity;
		TinyBufferArray<BYTE>	m_io;
	};
	//////////////////////////////////////////////////////////////////////////
	class TSH264Parser : public TSParser
	{
	public:
		TSH264Parser();
		virtual ~TSH264Parser();
	public:
		BYTE	GetStreamType() const OVERRIDE;
		BOOL	Parse(TS_BLOCK& block) OVERRIDE;
	private:
		H264Parser		m_parser;
		vector<BYTE>	m_vsc;
	};
	//////////////////////////////////////////////////////////////////////////
	class TSAACParser : public TSParser
	{
	public:
		TSAACParser();
		virtual ~TSAACParser();
	public:
		BYTE	GetStreamType() const OVERRIDE;
		BOOL	Parse(TS_BLOCK& block) OVERRIDE;
	private:
		BOOL	ParseADTS(BYTE* bits, INT size);
	private:
		vector<BYTE>		m_asc;
	};
	//////////////////////////////////////////////////////////////////////////
	class TS_PACKET_STREAM
	{
	public:
		BYTE StreamType : 8;
		BYTE Reserved1 : 3;
		USHORT ElementaryPID : 13;
		BYTE Reserved2 : 4;
		USHORT ESInfoLength : 12;
		INT	Slices;
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
		BOOL	ReadPES(TS_PACKET_STREAM* stream, TS_PACKET_PES& myPES, TS_BLOCK& block, const BYTE* bits, INT offset);
		BOOL	ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, const BYTE* bits);
		BOOL	ReadPAT(TS_PACKET_PAT& myPAT, TinyArray<TS_PACKET_PROGRAM>& programs, const BYTE* bits);
		BOOL	ReadPTM(TS_PACKET_PMT& myPTM, TinyArray<TS_PACKET_STREAM*>& streams, const BYTE* bits);
	private:
		BYTE							m_bits[TS_PACKET_SIZE];
		INT								m_versionNumber;
		INT								m_continuityCounter;
		TinyComPtr<IStream>				m_stream;
		TinyArray<TS_PACKET_STREAM*>	m_streams;
		TinyArray<TS_PACKET_PROGRAM>	m_programs;
	};
}


