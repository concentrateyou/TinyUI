#include "stdafx.h"
#include "H264Parser.h"

namespace Decode
{
	H264Parser::H264Parser()
		:m_bits(NULL),
		m_size(0),
		m_count(0)
	{

	}
	H264Parser::~H264Parser()
	{

	}
	BOOL H264Parser::FindCode(const BYTE* bits, LONG size, LONG& offset, BYTE& code)
	{
		LONG remainingBytes = size;
		while (remainingBytes >= 3)
		{
			if (bits[0] == 0x00 && bits[1] == 0x00 && bits[2] == 0x01)
			{
				offset = size - remainingBytes;
				code = 3;
				if (offset > 0 && *(bits - 1) == 0x00)
				{
					--offset;
					++code;
				}
				return TRUE;
			}
			++bits;
			--remainingBytes;
		}
		offset = size - remainingBytes;
		code = 0;
		return FALSE;
	}
	BOOL H264Parser::Parse(const BYTE* bits, LONG size)
	{
		if (!bits || size <= 0)
			return FALSE;
		m_bits = bits;
		m_size = size;
		H264NALU s;
		ZeroMemory(&s, sizeof(s));
		for (;;)
		{
			BYTE code = 0;
			LONG offset = 0;
			BOOL bF = FindCode(m_bits, m_size, offset, code);
			m_bits += (offset + code);
			m_size -= (offset + code);
			if (!bF)
				break;
			if (m_count > 0)
			{
				s.size = offset;
				switch (s.Type)
				{
				case NALU_TYPE_AUD://·Ö¸ô·û
					break;
				case NALU_TYPE_SPS:
					ParseSPS(s.bits, s.size);
					break;
				case NALU_TYPE_PPS:
					ParsePPS(s.bits, s.size);
					break;
				}
			}
			s.Type = m_bits[0] & 0x1F;
			s.bits = m_bits;
			++m_count;
		}
		return TRUE;
	}
	BOOL H264Parser::ParseSPS(const BYTE* bits, LONG size)
	{
		m_reader.Initialize(bits, size);
		INT data = 0;
		H264SPS sps = { 0 };
		if (!m_reader.ReadBits(8, &sps.profile_idc))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set0_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set1_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set2_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set3_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set4_flag))
			return FALSE;
		if (!m_reader.ReadBits(1, &sps.constraint_set5_flag))
			return FALSE;
		if (!m_reader.ReadBits(2, &data))
			return FALSE;
		if (!m_reader.ReadBits(8, &sps.level_idc))
			return FALSE;
		if (!m_reader.ReadUE(&sps.seq_parameter_set_id))
			return FALSE;
		if (sps.seq_parameter_set_id >= 32)
			return FALSE;
		if (sps.profile_idc == 100 || sps.profile_idc == 110 ||
			sps.profile_idc == 122 || sps.profile_idc == 244 ||
			sps.profile_idc == 44 || sps.profile_idc == 83 ||
			sps.profile_idc == 86 || sps.profile_idc == 118 ||
			sps.profile_idc == 128)
		{
			if (m_reader.ReadUE(&sps.chroma_format_idc))
				return FALSE;
			if (sps.chroma_format_idc >= 4)
				return FALSE;
			if (sps.chroma_format_idc == 3)
			{
				if (m_reader.ReadBits(1, &sps.separate_colour_plane_flag))
					return FALSE;
			}
			if (m_reader.ReadUE(&sps.bit_depth_luma_minus8))
				return FALSE;
			if (sps.bit_depth_luma_minus8 >= 7)
				return FALSE;
			if (m_reader.ReadUE(&sps.bit_depth_chroma_minus8))
				return FALSE;
			if (sps.bit_depth_chroma_minus8 >= 7)
				return FALSE;
			if (m_reader.ReadBits(1, &sps.qpprime_y_zero_transform_bypass_flag))
				return FALSE;
			if (m_reader.ReadBits(1, &sps.seq_scaling_matrix_present_flag))
				return FALSE;
		}
		return TRUE;
	}
	BOOL H264Parser::ParsePPS(const BYTE* bits, LONG size)
	{
		return TRUE;
	}
}

