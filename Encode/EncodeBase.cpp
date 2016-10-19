#include "stdafx.h"
#include "EncodeBase.h"

namespace Encode
{
	EncodeBase::EncodeBase()
		:m_dwTimeOffset(0),
		m_dwTimestamp(0),
		m_dwTimeDen(0),
		m_dwTimeNum(0)
	{
	}


	EncodeBase::~EncodeBase()
	{
	}

	BOOL EncodeBase::Open()
	{
		return TRUE;
	}
	void  EncodeBase::Close()
	{

	}
	BOOL  EncodeBase::Encode()
	{
		return TRUE;
	}
	BOOL  EncodeBase::AddFilter()
	{
		return TRUE;
	}
	BOOL  EncodeBase::RemoveFilter()
	{
		return TRUE;
	}
}