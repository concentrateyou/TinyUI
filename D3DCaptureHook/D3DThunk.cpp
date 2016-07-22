#include "stdafx.h"
#include "D3DThunk.h"

namespace D3D
{
	CThunk::CThunk()
	{
		pThunk = NULL;
	}
	CThunk::~CThunk()
	{
		if (pThunk)
		{
			delete pThunk;
		}
	}
	BOOL CThunk::Initialize(DWORD_PTR proc, void *pThis)
	{
		if (pThunk == NULL)
		{
			pThunk = new _callthunk;
			if (pThunk == NULL)
			{
				return FALSE;
			}
		}
		return pThunk->Initialize(proc, pThis);
	}
	void* CThunk::GetCodeAddress()
	{
		return pThunk->GetCodeAddress();
	}
	////////////////////////////////////////////////////////////////////////////
	BOOL CHookThunk::Initialize(FARPROC proc, void* pThis)
	{
		return thunk.Initialize((DWORD_PTR)proc, pThis);
	}
	FARPROC CHookThunk::GetPROC()
	{
		return reinterpret_cast<FARPROC>(thunk.GetCodeAddress());
	}
}