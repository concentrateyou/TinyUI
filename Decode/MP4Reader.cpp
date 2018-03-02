#include "stdafx.h"
#include "MP4Reader.h"

namespace Decode
{
	static const INT AACLCObjectType = 2;
	static const INT AACSBRObjectType = 5;
	static const INT AACPSObjectType = 29;

	MP4Reader::MP4Reader()
	{
	}


	MP4Reader::~MP4Reader()
	{
	}
	BOOL MP4Reader::OpenFile(LPCSTR pzFile)
	{

		return TRUE;
	}
}