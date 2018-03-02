#pragma once
#include "Common.h"
#include "IO/TinyBitReader.h"
#include <vector>
using namespace std;
using namespace TinyUI;
using namespace TinyUI::IO;

namespace Decode
{
	/// <summary>
	/// AAC½âÎö
	/// </summary>
	class AACParser
	{
		DISALLOW_COPY_AND_ASSIGN(AACParser)
	public:
		AACParser();
		~AACParser();
		BOOL Parser(const BYTE* bits, LONG size);
	private:
		BYTE			m_profile;
		BYTE			m_frequency;
		BYTE			m_channel;
		TinyBitReader	m_reader;
	};
}



