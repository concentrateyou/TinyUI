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
	/// AVC½âÎö
	/// </summary>
	class AVCParser
	{
		DISALLOW_COPY_AND_ASSIGN(AVCParser)
	public:
		AVCParser();
		~AVCParser();
	private:
		TinyBitReader	m_reader;
	};
}



