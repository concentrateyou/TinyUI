#include "stdafx.h"
#include "H264Reader.h"

namespace Decode
{
	BOOL FindStartCode(BYTE* bits, INT size)
	{
		for (INT i = 0;i < size - 1;i++)
		{
			if (bits[i] != 0)
				return FALSE;
		}
		if (bits[size - 1] != 1)
			return FALSE;
		return TRUE;
	}
	H264Reader::H264Reader()
		:m_count(0)
	{

	}
	H264Reader::~H264Reader()
	{

	}
	BOOL H264Reader::Open(LPCSTR pzFile)
	{
		this->Close();
		HRESULT hRes = SHCreateStreamOnFileA(pzFile, STGM_READ | STGM_FAILIFTHERE, &m_stream);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL H264Reader::ReadNALU(NALU& nalu)
	{
		ULONG ls = 0;
		BYTE bits[4];
		HRESULT hRes = S_OK;
		ZeroMemory(&nalu, sizeof(nalu));
		m_io.SetOffset(0);
		if (m_io.RemainingCapacity() <= 0)
			m_io.SetCapacity(m_io.capacity() + DEFAULT_H264_BUFFER_SIZE);
		if (m_count <= 0)
		{
			for (;;)
			{
				hRes = m_stream->Read(bits, sizeof(BYTE) * 3, &ls);
				if (hRes != S_OK || ls <= 0)
					return FALSE;
				if (FindStartCode(bits, 3))
					break;
				hRes = m_stream->Read(&bits[3], sizeof(BYTE), &ls);
				if (hRes != S_OK || ls <= 0)
					return FALSE;
				if (FindStartCode(bits, 4))
					break;
			}
		}
		hRes = m_stream->Read(&bits[0], sizeof(BYTE), &ls);
		if (hRes != S_OK || ls <= 0)
			return FALSE;
		if (m_io.RemainingCapacity() <= 0)
			m_io.SetCapacity(m_io.capacity() + DEFAULT_H264_BUFFER_SIZE);
		memcpy_s(m_io.data(), 1, &bits[0], 1);
		m_io.SetOffset(m_io.offset() + 1);
		nalu.Forbidden = bits[0] & 0x80;
		nalu.Reference = bits[0] & 0x60;
		nalu.Type = bits[0] & 0x1F;
		for (;;)
		{
			hRes = m_stream->Read(bits, sizeof(BYTE) * 3, &ls);
			if (hRes != S_OK || ls <= 0)
				return FALSE;
			if (FindStartCode(bits, 3))
				break;
			if (m_io.RemainingCapacity() <= 0)
				m_io.SetCapacity(m_io.capacity() + DEFAULT_H264_BUFFER_SIZE);
			memcpy_s(m_io.data(), 3, bits, 3);
			m_io.SetOffset(m_io.offset() + 3);
			hRes = m_stream->Read(&bits[3], sizeof(BYTE), &ls);
			if (hRes != S_OK || ls <= 0)
				return FALSE;
			if (FindStartCode(bits, 4))
				break;
			if (m_io.RemainingCapacity() <= 0)
				m_io.SetCapacity(m_io.capacity() + DEFAULT_H264_BUFFER_SIZE);
			memcpy_s(m_io.data(), 1, bits, 1);
			m_io.SetOffset(m_io.offset() + 1);
		}
		++m_count;
		nalu.bits = reinterpret_cast<BYTE*>(m_io.StartOfBuffer());
		nalu.size = m_io.offset();
		return TRUE;
	}
	BOOL H264Reader::Close()
	{
		m_stream.Release();
		return TRUE;
	}
}

