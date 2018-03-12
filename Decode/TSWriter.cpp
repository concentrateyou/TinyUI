#include "stdafx.h"
#include "TSWriter.h"

namespace Decode
{
	static BYTE const StuffingBytes[TS_PACKET_SIZE] =
	{
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF
	};

	TSWriter::Stream::Stream(UINT16 sPID)
		:m_sPID(sPID),
		m_continuityCounter(0)
	{
		ZeroMemory(m_bits, TS_PACKET_SIZE);
	}
	TSWriter::Stream::~Stream()
	{

	}

	UINT16 TSWriter::Stream::GetPID()
	{
		return m_sPID;
	}

	BOOL TSWriter::Stream::WritePacketHeader(BOOL  bPayloadUnitStartIndicator, UINT32&  sizePL, BOOL  bPCR, UINT64 sPCR, TinyComPtr<IStream>& stream)
	{
		BYTE header[4];
		ZeroMemory(header, 4);
		header[0] = TS_SYNC_BYTE;	//sync_byte
		header[1] = (BYTE)(((bPayloadUnitStartIndicator ? 1 : 0) << 6) | (m_sPID >> 8));
		header[2] = m_sPID & 0xFF;	//PID
		UINT32 sizeAF = 0;
		if (bPCR)
		{
			sizeAF += 2 + TS_PCR_ADAPTATION_SIZE;
		}
		if ((sizePL + sizeAF) > TS_PACKET_PAYLOAD_SIZE)
		{
			sizePL = TS_PACKET_PAYLOAD_SIZE - sizeAF;
		}
		if ((sizePL + sizeAF) < TS_PACKET_PAYLOAD_SIZE)
		{
			sizeAF = TS_PACKET_PAYLOAD_SIZE - sizePL;
		}
		ULONG ls = 0;
		HRESULT hRes = S_OK;
		if (sizeAF == 0)
		{
			header[3] = (1 << 4) | ((m_continuityCounter++) & 0x0F);
			hRes = stream->Write(header, 4, &ls);
			if (hRes != S_OK || ls != 4)
				return FALSE;
		}
		else
		{
			header[3] = (3 << 4) | ((m_continuityCounter++) & 0x0F);
			hRes = stream->Write(header, 4, &ls);
			if (hRes != S_OK || ls != 4)
				return FALSE;
			if (sizeAF == 1)
			{
				INT s = 0;
				hRes = stream->Write(&s, 1, &ls);
				if (hRes != S_OK || ls != 4)
					return FALSE;
			}
			else
			{
				INT s = sizeAF - 1;
				hRes = stream->Write(&s, 1, &ls);
				if (hRes != S_OK || ls != 4)
					return FALSE;
				s = bPCR ? (1 << 4) : 0;
				hRes = stream->Write(&s, 1, &ls);
				if (hRes != S_OK || ls != 4)
					return FALSE;
				UINT32 sizePCR = 0;
				if (bPCR)
				{
					sizePCR = TS_PCR_ADAPTATION_SIZE;
					UINT64 basePCR = sPCR / 300;
					UINT32 extensionPCR = (UINT32)(sPCR % 300);
					TinyBitWriter writer;
					writer.Initialize(m_bits, sizePCR);
					writer.WriteBits((UINT32)(basePCR >> 32), 1);
					writer.WriteBits((UINT32)basePCR, 32);
					writer.WriteBits(0x3F, 6);
					writer.WriteBits(extensionPCR, 9);
					hRes = stream->Write(m_bits, sizePCR, &ls);
					if (hRes != S_OK || ls != sizePCR)
						return FALSE;
				}
				if (sizeAF > 2)
				{
					hRes = stream->Write(StuffingBytes, (sizeAF - sizePCR - 2), &ls);
					if (hRes != S_OK || ls != (sizeAF - sizePCR - 2))
						return FALSE;
				}
			}
		}
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	TSWriter::SampleStream::SampleStream(UINT16 sPID, BYTE streamType, UINT16 streamID, UINT32 timescale, const BYTE* descriptor, UINT32 descriptorSize)
		:TSWriter::Stream(sPID),
		m_streamType(streamType),
		m_streamID(streamID),
		m_timescale(timescale),
		m_descriptorSize(descriptorSize)
	{
		m_descriptor.Reset(new BYTE[descriptorSize]);
		ASSERT(m_descriptor);
		memcpy_s(m_descriptor, descriptorSize, descriptor, descriptorSize);
	}
	BOOL TSWriter::SampleStream::WritePES(const BYTE* data, UINT size, UINT64 dts, BOOL bDTS, UINT64 pts, BOOL bPCR, TinyComPtr<IStream>& stream)
	{
		UINT32 sizePES = 14 + (bDTS ? 5 : 0);
		TinyScopedArray<BYTE> bits(new BYTE[sizePES]);
		if (!bits)
			return FALSE;
		TinyBitWriter writer;
		writer.Initialize(bits, size);
		writer.WriteBits(24, 0x000001);
		writer.WriteBits(8, m_streamID);
		INT s = (m_streamID == 0xe0) ? 0 : (size + sizePES - 6);
		writer.WriteBits(16, s);
		writer.WriteBits(2, 2);						// '10'
		writer.WriteBits(2, 0);						// PES_scrambling_control
		writer.WriteBits(1, 0);						// PES_priority
		writer.WriteBits(1, 1);						// data_alignment_indicator
		writer.WriteBits(1, 0);						// copyright
		writer.WriteBits(1, 0);						// original_or_copy
		writer.WriteBits(2, bDTS ? 3 : 2);			// PTS_DTS_flags
		writer.WriteBits(1, 0);						// ESCR_flag
		writer.WriteBits(1, 0);						// ES_rate_flag
		writer.WriteBits(1, 0);						// DSM_trick_mode_flag
		writer.WriteBits(1, 0);						// additional_copy_info_flag
		writer.WriteBits(1, 0);						// PES_CRC_flag
		writer.WriteBits(1, 0);						// PES_extension_flag
		writer.WriteBits(8, sizePES - 9);			// PES_header_data_length
		writer.WriteBits(4, bDTS ? 3 : 2);			// '0010' or '0011'
		writer.WriteBits(3, (UINT32)(pts >> 30));	// PTS[32..30]
		writer.WriteBits(1, 1);						// marker_bit
		writer.WriteBits(15, (UINT32)(pts >> 15));	// PTS[29..15]
		writer.WriteBits(1, 1);						// marker_bit
		writer.WriteBits(15, (UINT32)pts);			// PTS[14..0]
		writer.WriteBits(1, 1);						// market_bit
		if (bDTS)
		{
			writer.WriteBits(4, 1);						// '0001'
			writer.WriteBits(3, (UINT32)(dts >> 30));	// DTS[32..30]
			writer.WriteBits(1, 1);						// marker_bit
			writer.WriteBits(15, (UINT32)(dts >> 15));	// DTS[29..15]
			writer.WriteBits(1, 1);						// marker_bit
			writer.WriteBits(15, (UINT32)dts);			// DTS[14..0]
			writer.WriteBits(1, 1);						// market_bit
		}
		BOOL bF = TRUE;
		size += sizePES;
		while (size)
		{
			UINT32 sizePL = size;
			if (sizePL > TS_PACKET_PAYLOAD_SIZE)
				sizePL = TS_PACKET_PAYLOAD_SIZE;
			if (bF)
			{
				WritePacketHeader(bF, sizePL, bPCR, (bDTS ? dts : pts) * 300, stream);
				bF = FALSE;
				ULONG ls = 0;
				HRESULT hRes = stream->Write(bits, sizePES, &ls);
				if (hRes != S_OK || ls != sizePES)
					return FALSE;
				hRes = stream->Write(data, sizePL - sizePES, &ls);
				if (hRes != S_OK || ls != (sizePL - sizePES))
					return FALSE;
				data += sizePL - sizePES;
			}
			else
			{
				WritePacketHeader(bF, sizePL, FALSE, 0, stream);
				ULONG ls = 0;
				HRESULT hRes = stream->Write(data, sizePL, &ls);
				if (hRes != S_OK || ls != sizePL)
					return FALSE;
				data += sizePL;
			}
			size -= sizePL;
		}
		return TRUE;
	}
	void TSWriter::SampleStream::SetType(BYTE streamType)
	{
		m_streamType = streamType;
	}
	BYTE TSWriter::SampleStream::GetType() const
	{
		return m_streamType;
	}
	void TSWriter::SampleStream::SetDescriptor(const BYTE* descriptor, UINT32 descriptorSize)
	{
		if (descriptorSize != m_descriptorSize)
		{
			m_descriptorSize = descriptorSize;
			m_descriptor.Reset(new BYTE[descriptorSize]);
		}
		memcpy_s(m_descriptor, m_descriptorSize, descriptor, m_descriptorSize);
	}
	UINT32 TSWriter::SampleStream::GetDescriptorSize() const
	{
		return m_descriptorSize;
	}
	BYTE*	TSWriter::SampleStream::GetDescriptor()
	{
		return m_descriptor;
	}
	//////////////////////////////////////////////////////////////////////////
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
		m_tsPAT.Reset(new Stream(0));
		if (!m_tsPAT)
			return FALSE;
		m_tsPMT.Reset(new Stream(0x100));
		if (!m_tsPMT)
			return FALSE;
		return TRUE;
	}
	BOOL TSWriter::SetAudioStream(UINT32 timescale, BYTE  streamType, UINT16  streamID, UINT16 sPID, const BYTE* descriptor, UINT32 descriptorSize)
	{
		m_tsAudio.Reset(new SampleStream(sPID, streamType, streamID, timescale, descriptor, descriptorSize));
		return m_tsAudio != NULL;
	}
	BOOL TSWriter::SetVideoStream(UINT32 timescale, BYTE  streamType, UINT16  streamID, UINT16 sPID, const BYTE* descriptor, UINT32  descriptorSize)
	{
		m_tsVideo.Reset(new SampleStream(sPID, streamType, streamID, timescale, descriptor, descriptorSize));
		return m_tsVideo != NULL;
	}

	BOOL TSWriter::WritePAT()
	{
		if (!m_tsPAT || !m_tsPMT)
			return FALSE;
		UINT32 sizePL = TS_PACKET_PAYLOAD_SIZE;
		if (!m_tsPAT->WritePacketHeader(TRUE, sizePL, FALSE, 0, m_stream))
			return FALSE;
		TinyBitWriter writer;
		writer.Initialize(m_bits, TS_PACKET_SIZE);
		writer.WriteBits(8, 0);					// pointer
		writer.WriteBits(8, 0);					// table_id
		writer.WriteBits(1, 1);					// section_syntax_indicator
		writer.WriteBits(1, 0);					// '0'
		writer.WriteBits(2, 3);					// reserved
		writer.WriteBits(12, 13);				// section_length
		writer.WriteBits(16, 1);				// transport_stream_id
		writer.WriteBits(2, 3);					// reserved
		writer.WriteBits(5, 0);					// version_number
		writer.WriteBits(1, 1);					// current_next_indicator
		writer.WriteBits(8, 0);					// section_number
		writer.WriteBits(8, 0);					// last_section_number
		writer.WriteBits(16, 1);				// program number
		writer.WriteBits(3, 7);					// reserved
		writer.WriteBits(13, m_tsPMT->GetPID());// program_map_PID
		writer.WriteBits(32, ComputeCRC(m_bits + 1, 17 - 1 - 4));
		ULONG ls = 0;
		HRESULT hRes = m_stream->Write(m_bits, 17, &ls);
		if (hRes != S_OK || ls != 4)
			return FALSE;
		hRes = m_stream->Write(StuffingBytes, TS_PACKET_PAYLOAD_SIZE - 17, &ls);
		if (hRes != S_OK || ls != (TS_PACKET_PAYLOAD_SIZE - 17))
			return FALSE;
		return TRUE;
	}
	BOOL TSWriter::WritePMT()
	{
		if (!m_tsPAT || !m_tsPMT)
			return FALSE;
		TinyBitWriter writer;
		writer.Initialize(m_bits, TS_PACKET_SIZE);
		UINT32 sectionSize = 13;
		UINT32 pcrPID = 0;
		if (m_tsAudio)
		{
			sectionSize += 5 + m_tsAudio->GetDescriptorSize();
			pcrPID = m_tsAudio->GetPID();
		}
		if (m_tsVideo)
		{
			sectionSize += 5 + m_tsVideo->GetDescriptorSize();
			pcrPID = m_tsVideo->GetPID();
		}
		writer.WriteBits(8, 0);        // pointer
		writer.WriteBits(8, 2);        // table_id
		writer.WriteBits(1, 1);        // section_syntax_indicator
		writer.WriteBits(1, 0);        // '0'
		writer.WriteBits(2, 3);        // reserved
		writer.WriteBits(12, sectionSize); // section_length
		writer.WriteBits(16, 1);       // program_number
		writer.WriteBits(2, 3);        // reserved
		writer.WriteBits(5, 0);        // version_number
		writer.WriteBits(1, 1);        // current_next_indicator
		writer.WriteBits(8, 0);        // section_number
		writer.WriteBits(8, 0);        // last_section_number
		writer.WriteBits(3, 7);        // reserved
		writer.WriteBits(13, pcrPID); // PCD_PID
		writer.WriteBits(4, 0xF);      // reserved
		writer.WriteBits(12, 0);       // program_info_length
		if (m_tsAudio)
		{
			writer.WriteBits(8, m_tsAudio->GetType());                // stream_type
			writer.WriteBits(3, 0x7);                                  // reserved
			writer.WriteBits(13, m_tsAudio->GetPID());                   // elementary_PID
			writer.WriteBits(4, 0xF);                                  // reserved
			writer.WriteBits(12, m_tsAudio->GetDescriptorSize()); // ES_info_length
			for (UINT32 i = 0; i < m_tsAudio->GetDescriptorSize(); i++)
			{
				writer.WriteBits(8, m_tsAudio->GetDescriptor()[i]);
			}
		}
		if (m_tsVideo)
		{
			writer.WriteBits(8, m_tsVideo->GetType());                // stream_type
			writer.WriteBits(0x7, 3);                                  // reserved
			writer.WriteBits(13, m_tsVideo->GetPID());                   // elementary_PID
			writer.WriteBits(4, 0xF);                                  // reserved
			writer.WriteBits(12, m_tsVideo->GetDescriptorSize()); // ES_info_length
			for (UINT32 i = 0; i < m_tsVideo->GetDescriptorSize(); i++)
			{
				writer.WriteBits(8, m_tsVideo->GetDescriptor()[i]);
			}
		}
		writer.WriteBits(32, ComputeCRC(m_bits + 1, sectionSize - 1)); // CRC

		ULONG ls = 0;
		HRESULT hRes = m_stream->Write(m_bits, 17, &ls);
		if (hRes != S_OK || ls != 4)
			return FALSE;
		hRes = m_stream->Write(m_bits, sectionSize + 4, &ls);
		if (hRes != S_OK || ls != (sectionSize + 4))
			return FALSE;
		hRes = m_stream->Write(StuffingBytes, TS_PACKET_PAYLOAD_SIZE - (sectionSize + 4), &ls);
		if (hRes != S_OK || ls != (TS_PACKET_PAYLOAD_SIZE - (sectionSize + 4)))
			return FALSE;
		return TRUE;
	}
}
