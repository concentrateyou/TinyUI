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
		:m_count(0),
		m_mask(4)
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
		m_io.SetOffset(0);
		if (m_io.RemainingCapacity() <= 0)
			m_io.SetCapacity(m_io.capacity() + DEFAULT_H264_BUFFER_SIZE);
		return TRUE;
	}
	BOOL H264Reader::ReadNALU(NALU& nalu)
	{
		ULONG ls = 0;
		HRESULT hRes = S_OK;
		ZeroMemory(&nalu, sizeof(nalu));
		BYTE bits[4];
		ZeroMemory(bits, 4);
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
				{
					m_mask = 0x000001;
					break;
				}
				hRes = m_stream->Read(&bits[3], sizeof(BYTE), &ls);
				if (hRes != S_OK || ls <= 0)
					return FALSE;
				if (FindStartCode(bits, 4))
				{
					m_mask = 0x00000001;
					break;
				}
			}
		}
		INT mask = 0;
		INT index = 3;
		for (;;)
		{
			if (mask & m_mask)
				break;
			hRes = m_stream->Read(bits, sizeof(BYTE) * 1, &ls);
			if (hRes != S_OK || ls <= 0)
				return FALSE;
			if (bits[0] != 0x00 && bits[0] != 0x01)
			{
				mask = 0;
				index = 3;
			}
			else
			{
				mask = (bits[0] & 0xFF) << (index * 8);
				index--;
			}
			if (m_io.RemainingCapacity() <= 0)
				m_io.SetCapacity(m_io.capacity() + DEFAULT_H264_BUFFER_SIZE);
			memcpy_s(m_io.data(), 1, bits, 1);
			m_io.SetOffset(m_io.offset() + 1);
		}
		m_count++;
		nalu.bits = (BYTE*)m_io.StartOfBuffer();
		nalu.Forbidden = bits[0] & 0x80;
		nalu.Reference = bits[0] & 0x60;
		nalu.Type = bits[0] & 0x1F;
		nalu.size = m_io.offset() - (m_mask == 0x00000001 ? 4 : 3);
		return TRUE;
	}
	BOOL H264Reader::Close()
	{
		m_stream.Release();
		return TRUE;
	}
}

