#pragma once
#include "Common.h"
#include <vector>

namespace Decode
{
	/// <summary>
	/// TS�ļ���װ
	/// </summary>
	class TSWriter
	{
		DISALLOW_COPY_AND_ASSIGN(TSWriter)
	public:
		TSWriter();
		virtual ~TSWriter();
	public:
		BOOL WritePAT();
		BOOL WritePMT();
		BOOL WritePES();
	};
}


