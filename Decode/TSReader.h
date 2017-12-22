#pragma once
#include "Common.h"

namespace Decode
{
	/// <summary>
	/// TSÎÄ¼þ½âÎö
	/// </summary>
	class TSReader
	{
		DISALLOW_COPY_AND_ASSIGN(TSReader)
	public:
		TSReader();
		virtual ~TSReader();
		BOOL	OpenFile(LPCSTR pzFile);
		BOOL	Close();
	private:
		TinyComPtr<IStream>		m_stream;
	};
}


