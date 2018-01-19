#pragma once
#include "Common.h"
#include <vector>
using namespace std;
using namespace TinyUI;

namespace Decode
{
	/// <summary>
	/// H264½âÎö
	/// </summary>
	class H264Parser
	{
		DISALLOW_COPY_AND_ASSIGN(H264Parser)
	public:
		H264Parser();
		virtual ~H264Parser();
		BOOL Parse(const BYTE* bits, LONG size);
	};
}
