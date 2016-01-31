#include "../stdafx.h"
#include "TinyAPIHook.h"

namespace TinyUI
{
	TinyAPIHook::TinyAPIHook()
	{
	}
	TinyAPIHook::~TinyAPIHook()
	{

	}
	TinyAPIHook* TinyAPIHook::GetInstance()
	{
		if (m_pHook == NULL)
		{
			TinyLock lock;
			lock.Acquire();
			if (m_pHook == NULL)
			{
				m_pHook = new TinyAPIHook();
			}
			lock.Release();
		}
		return m_pHook;
	}
}