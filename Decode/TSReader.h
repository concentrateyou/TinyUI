#pragma once
#include "Common.h"
#include "H264Parser.h"
#include "HTTPStream.h"

namespace Decode
{
	class TSParser
	{
		DISALLOW_COPY_AND_ASSIGN(TSParser)
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
		vector<BYTE>			m_config;
	};
	/// <summary>
	/// TS H264解析
	/// </summary>
	class TSH264Parser : public TSParser
	{
		DISALLOW_COPY_AND_ASSIGN(TSH264Parser)
	public:
		TSH264Parser(ConfigCallback& callback);
		virtual ~TSH264Parser();
	public:
		BYTE	GetStreamType() const OVERRIDE;
		BOOL	Parse(TS_BLOCK& block) OVERRIDE;
	private:
		H264Parser		m_parser;
		ConfigCallback	m_callback;
	};
	/// <summary>
	/// TS AAC解析
	/// </summary>
	class TSAACParser : public TSParser
	{
		DISALLOW_COPY_AND_ASSIGN(TSAACParser)
	public:
		TSAACParser(ConfigCallback& callback);
		virtual ~TSAACParser();
	public:
		BYTE	GetStreamType() const OVERRIDE;
		BOOL	Parse(TS_BLOCK& block) OVERRIDE;
		static void	ParseAAC(TS_BLOCK& block, TinyLinkList<TS_BLOCK>& audios);
	private:
		BOOL	ParseADTS(BYTE* bits, LONG size);
	private:
		FLOAT			m_timestamp;
		AACAudioConfig	m_lastConfig;
		ConfigCallback	m_callback;
	};
	//////////////////////////////////////////////////////////////////////////
	class TS_PACKET_STREAM
	{
		friend class TSReader;
	public:
		BYTE StreamType : 8;
		BYTE Reserved1 : 3;
		USHORT ElementaryPID : 13;
		BYTE Reserved2 : 4;
		USHORT ESInfoLength : 12;
	public:
		LONGLONG PTS;
		LONGLONG DTS;
	public:
		TS_PACKET_STREAM();
		~TS_PACKET_STREAM();
		BOOL operator == (const TS_PACKET_STREAM& other);
		TSParser* GetParser(ConfigCallback& callback);
	private:
		LONGLONG				m_baseDTS;
		LONGLONG				m_basePTS;
		INT						m_continuityCounter;
		TinyScopedPtr<TSParser>	m_parser;
	};
	/// <summary>
	/// TS文件解析
	/// </summary>
	class TSReader
	{
		DISALLOW_COPY_AND_ASSIGN(TSReader)
	public:
		TSReader();
		virtual ~TSReader();
		void	SetConfigCallback(ConfigCallback&& callback);
		BOOL	OpenFile(LPCSTR pzFile);
		BOOL	OpenURL(LPCSTR pzURL);
		BOOL	Close();
		BOOL	ReadBlock(TS_BLOCK& block);
	private:
		BOOL	ReadPacket(TS_PACKEG_HEADER& header, TS_BLOCK& block);
		BOOL	ReadPES(TS_PACKET_STREAM* stream, TS_PACKET_PES& myPES, TS_BLOCK& block, const BYTE* bits, INT offset);
		BOOL	ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, const BYTE* bits);
		BOOL	ReadPAT(TS_PACKET_PAT& myPAT, TinyArray<TS_PACKET_PROGRAM>& programs, const BYTE* bits);
		BOOL	ReadPMT(TS_PACKET_PMT& myPTM, TinyArray<TS_PACKET_STREAM*>& streams, const BYTE* bits);
		BOOL	ReadSDT(TS_PACKET_SDT& mySDT, const BYTE* bits);
	private:
		LONG							m_size;
		BYTE							m_bits[TS_PACKET_SIZE];
		INT								m_versionNumberPAT;
		INT								m_versionNumberPMT;
		ConfigCallback					m_configCallback;
		TS_PACKET_STREAM*				m_original;
		TinyLinkList<TS_BLOCK>			m_audios;
		TinyComPtr<IStream>				m_stream;
		TinyArray<TS_PACKET_STREAM*>	m_streams;
		TinyArray<TS_PACKET_PROGRAM>	m_programs;
	};
}


