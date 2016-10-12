#pragma once
#include "Common.h"

class EncodeBase
{
public:
	EncodeBase();
	virtual ~EncodeBase();
public:
	/*virtual BOOL  Open();
	virtual void  Close();
	virtual BOOL  Encode();
	virtual BOOL  AddFilter();
	virtual BOOL  RemoveFilter();*/
protected:
	DWORD	m_dwTimeOffset;
	DWORD	m_dwLastTimestamp;
	DWORD	m_dwTimeDen;
	DWORD	m_dwTimeNum;
};

