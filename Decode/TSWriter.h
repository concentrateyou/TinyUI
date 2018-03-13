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
	private:
		class Stream
		{
			DISALLOW_COPY_AND_ASSIGN(Stream)
		public:
			Stream(UINT16 sPID);
			virtual ~Stream();
			UINT16 GetPID();
			BOOL WritePacketHeader(BOOL  bPayloadUnitStartIndicator, UINT32&  sizePayload, BOOL  bPCR, UINT64 sPCR, TinyComPtr<IStream>& stream);
		private:
			BYTE	m_bits[TS_PACKET_SIZE];
			UINT16  m_sPID;
			UINT32	m_continuityCounter;
		};
		class SampleStream : public Stream
		{
			DISALLOW_COPY_AND_ASSIGN(SampleStream)
		public:
			SampleStream(UINT16 sPID, BYTE streamType, UINT16 streamID, UINT32 timescale, const BYTE* descriptor, UINT32 descriptorSize);
			virtual BOOL WritePES(const BYTE* data, UINT size, UINT64 dts, BOOL bDTS, UINT64 pts, BOOL bPCR, TinyComPtr<IStream>& stream);
			void SetType(BYTE streamType);
			BYTE GetType() const;
			void SetDescriptor(const BYTE* descriptor, UINT32 descriptorSize);
			UINT32	GetDescriptorSize() const;
			BYTE*	GetDescriptor();
		private:
			BYTE					m_streamType;
			UINT16					m_streamID;
			UINT32					m_timescale;
			UINT32					m_descriptorSize;
			TinyScopedArray<BYTE>	m_descriptor;
		};
	public:
		TSWriter();
		virtual ~TSWriter();
		BOOL Create(LPCSTR pzFile);
		BOOL SetAudioStream(UINT32 timescale, BYTE  streamType, UINT16  streamID, UINT16 sPID, const BYTE* descriptor = NULL, UINT32 descriptorSize = 0);
		BOOL SetVideoStream(UINT32 timescale, BYTE  streamType, UINT16  streamID, UINT16 sPID, const BYTE* descriptor = NULL, UINT32  descriptorSize = 0);
		BOOL WriteAAC(const BYTE* raw, LONG size, AACAudioConfig& config, LONGLONG time);
		BOOL WriteAAC(const BYTE* bits, LONG size, LONGLONG time);
		BOOL WriteH264(const BYTE* raw, LONG size, const vector<BYTE>& sps, const vector<BYTE>& pps, LONGLONG time);
	private:
		BOOL WritePAT();
		BOOL WritePMT();
	private:
		BYTE							m_bits[TS_PACKET_SIZE];
		TinyComPtr<IStream>				m_stream;
		TinyScopedPtr<Stream>			m_tsPMT;
		TinyScopedPtr<Stream>			m_tsPAT;
		TinyScopedPtr<SampleStream>		m_audio;
		TinyScopedPtr<SampleStream>		m_video;
	};
}


