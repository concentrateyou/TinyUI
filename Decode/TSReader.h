#pragma once
#include "Common.h"
#include "H264Parser.h"
#include "HTTPStream.h"

namespace Decode
{
	struct Timing
	{
		DOUBLE PTS;
		DOUBLE DTS;
	};
	/// <summary>
	/// TS解析
	/// </summary>
	class TSParser
	{
		DISALLOW_COPY_AND_ASSIGN(TSParser)
	public:
		TSParser();
		virtual ~TSParser();
		virtual BYTE		GetStreamType() const = 0;
		virtual TS_ERROR	Parse(TS_BLOCK& block, Timing& timing) = 0;
		void	Add(BYTE* bits, INT size);
		void	Remove(INT size);
		void	Reset();
		BYTE*	data();
		INT		size() const;
	private:
		INT						m_capacity;
		TinyBufferArray<BYTE>	m_io;
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
		BYTE		GetStreamType() const OVERRIDE;
		TS_ERROR	Parse(TS_BLOCK& block, Timing& timing) OVERRIDE;
	private:
		H264Parser			m_parser;
		ConfigCallback		m_callback;
		TinyLinkList<Timing>m_timings;
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
		BYTE		GetStreamType() const OVERRIDE;
		TS_ERROR	Parse(TS_BLOCK& block, Timing& timing) OVERRIDE;
		const AACAudioConfig& GetAudioConfig() const;
		static void	ParseAAC(TS_BLOCK& block, TinyLinkList<TS_BLOCK>& audios, DOUBLE AACTimestamp);
	private:
		BOOL		ParseADTS(BYTE* bits, LONG size);
	private:
		AACAudioConfig		m_lastConfig;
		ConfigCallback		m_callback;
		TinyLinkList<Timing>m_timings;
	};
	//////////////////////////////////////////////////////////////////////////
	class TS_PACKET_STREAM : public TinyReference< TS_PACKET_STREAM >
	{
		friend class TSReader;
	public:
		BYTE StreamType : 8;
		BYTE Reserved1 : 3;
		USHORT ElementaryPID : 13;
		BYTE Reserved2 : 4;
		USHORT ESInfoLength : 12;
	public:
		DOUBLE PTS;
		DOUBLE DTS;
	public:
		TS_PACKET_STREAM(ConfigCallback&& callback);
		~TS_PACKET_STREAM();
		BOOL operator == (const TS_PACKET_STREAM& other);
		TSParser* GetParser();
	private:
		ConfigCallback			m_configCallback;
		TinyScopedPtr<TSParser>	m_parser;
	};
	/// <summary>
	/// TS文件解析,目前只支持AAC+H264
	/// </summary>
	class TSReader
	{
		DISALLOW_COPY_AND_ASSIGN(TSReader)
	public:
		TSReader();
		virtual ~TSReader();
		void	SetConfigCallback(ConfigCallback&& callback);
		BOOL	OpenMemory(const BYTE* bits, LONG size);
		BOOL	OpenFile(LPCSTR pzFile);
		BOOL	OpenURL(LPCSTR pzURL);
		BOOL	Close();
		BOOL	ReadBlock(TS_BLOCK& block);
	private:
		void	OnConfigCallback(const BYTE*, LONG, BYTE, LPVOID);
		BOOL	ReadPacket(TS_PACKEG_HEADER& header, TS_BLOCK& block);
		BOOL	ReadPES(TS_PACKET_STREAM* stream, TS_PACKET_PES& myPES, const BYTE* bits, INT offset, BOOL bPayloadUnitStartIndicator);
		BOOL	ReadAF(TS_PACKET_ADAPTATION_FIELD& myAF, const BYTE* bits);
		BOOL	ReadPAT(TS_PACKET_PAT& myPAT, TinyArray<TS_PACKET_PROGRAM>& programs, const BYTE* bits);
		BOOL	ReadPMT(TS_PACKET_PMT& myPTM, TinyArray<TinyScopedReferencePtr<TS_PACKET_STREAM>>& streams, const BYTE* bits);
		BOOL	ReadSDT(TS_PACKET_SDT& mySDT, const BYTE* bits);
	private:
		BYTE							m_bits[TS_PACKET_SIZE];
		INT								m_versionNumberPAT;
		INT								m_versionNumberPMT;
		DOUBLE							m_audioSR;//音频采样率HZ
		ConfigCallback					m_configCallback;
		TS_PACKET_STREAM*				m_previous;
		TinyLinkList<TS_BLOCK>			m_audios;
		TinyComPtr<IStream>				m_stream;
		TinyMap<USHORT, INT>			m_continuityCounterMap;
		TinyArray<TS_PACKET_PROGRAM>	m_programs;
		TinyArray<TinyScopedReferencePtr<TS_PACKET_STREAM>>	m_streams;
	};
}

namespace TinyFramework
{
	template<>
	class DefaultTraits < Decode::Timing >
	{
	public:
		static INT  Compare(const Decode::Timing& value1, const Decode::Timing& value2)
		{
			if (value1.PTS == value2.PTS && value1.DTS == value2.DTS)
				return 0;
			return 1;
		}
	};
}



