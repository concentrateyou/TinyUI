#include "stdafx.h"
#include "FLVDecode.h"

namespace FLVPlayer
{
	FLVDecode::FLVDecode(HWND hWND)
		:m_hWND(hWND)
	{

	}

	FLVDecode::~FLVDecode()
	{
		
	}
	BOOL FLVDecode::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVDecode::OnMessagePump, this));
	}
	BOOL FLVDecode::Close(DWORD dwMs)
	{
		return TRUE;
	}
	void FLVDecode::OnMessagePump()
	{

	}
}

