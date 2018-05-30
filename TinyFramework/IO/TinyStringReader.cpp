#include "../stdafx.h"
#include "TinyStringReader.h"
#include <algorithm>

namespace TinyFramework
{
	namespace IO
	{
		TinyStringReader::TinyStringReader(const string& Mystr)
			:m_Mystr(std::move(Mystr)),
			m_Mypos(0)
		{

		}
		TinyStringReader::~TinyStringReader()
		{

		}
		BOOL TinyStringReader::ReadLine(CHAR*& line1, CHAR*& line2)
		{
			if (m_Mystr.empty())
				return FALSE;
			string::size_type index = m_Mypos;
			while (index < m_Mystr.size())
			{
				CHAR s = m_Mystr[index];
				if (s == '\r' || s == '\n')
				{
					line1 = &m_Mystr[0] + m_Mypos;
					line2 = line1 + index - m_Mypos;
					m_Mypos = index + 1;
					if (s == '\r' &&
						m_Mypos < m_Mystr.size() &&
						m_Mystr[m_Mypos] == '\n')
					{
						m_Mypos++;
					}
					return TRUE;
				}
				index++;
			}
			if (index > m_Mypos)
			{
				line1 = &m_Mystr[0] + m_Mypos;
				line2 = line1 + index - m_Mypos;
				m_Mypos = index;
				return TRUE;
			}
			return FALSE;
		}
		string TinyStringReader::ReadLine()
		{
			if (m_Mystr.empty())
				return m_Mystr;
			string::size_type index = m_Mypos;
			while (index < m_Mystr.size())
			{
				CHAR s = m_Mystr[index];
				if (s == '\r' || s == '\n')
				{
					string line(&m_Mystr[0], m_Mypos, index - m_Mypos);
					m_Mypos = index + 1;
					if (s == '\r' &&
						m_Mypos < m_Mystr.size() &&
						m_Mystr[m_Mypos] == '\n')
					{
						m_Mypos++;
					}
					return line;
				}
				index++;
			}
			if (index > m_Mypos)
			{
				string line(&m_Mystr[0], m_Mypos, index - m_Mypos);
				m_Mypos = index;
				return line;
			}
			return string();
		}
	}
}
