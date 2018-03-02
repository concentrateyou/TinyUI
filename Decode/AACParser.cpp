#include "stdafx.h"
#include "AACParser.h"

namespace Decode
{
	AACParser::AACParser()
		:m_profile(0),
		m_frequency(0),
		m_channel(0)
	{
	}


	AACParser::~AACParser()
	{
	}

	BOOL AACParser::Parser(const BYTE* bits, LONG size)
	{
		if (!m_reader.Initialize(bits, size))
			return FALSE;
		return TRUE;
	}
}
