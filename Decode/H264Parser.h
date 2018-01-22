#pragma once
#include "Common.h"
#include <vector>
using namespace std;
using namespace TinyUI;

namespace Decode
{
	/// <summary>
	/// H264解析
	/// </summary>
	class H264Parser
	{
		DISALLOW_COPY_AND_ASSIGN(H264Parser)
	public:
		H264Parser();
		virtual ~H264Parser();
		BOOL Parse(const BYTE* bits, LONG size);
		static BOOL FindCode(const BYTE* bits, LONG size, LONG& offset, BYTE& code);//获得起始码
	private:
		BOOL ParseSPS(const BYTE* bits, LONG size);
		BOOL ParsePPS(const BYTE* bits, LONG size);
	private:
		const BYTE*				m_bits;
		LONG					m_size;
		LONG					m_count;
		TinyBitReader			m_reader;
	};
}
