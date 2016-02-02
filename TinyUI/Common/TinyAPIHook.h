#pragma once
#include "TinyCommon.h"
#include "TinySingle.h"
#include "TinyString.h"

namespace TinyUI
{
	/// <summary>
	/// API¹³×Ó
	/// </summary>
	class TinyAPIHook
	{
	public:
		TinyAPIHook();
		virtual ~TinyAPIHook();
	public:
		static TinyAPIHook* GetInstance();
	private:
		static TinyAPIHook* m_pHook;
	};
	TinyAPIHook* TinyAPIHook::m_pHook = NULL;

	/// <summary>
	/// APIº¯Êý
	/// </summary>
	class TinyAPIFunction
	{
		friend class TinyAPIHook;
	public:
		TinyAPIFunction(TinyAPIHook* pHook);
		~TinyAPIFunction();
	private:
		TinyAPIHook*		m_pAPIHook;
		/*PROC				m_pfnOrig;
		PROC				m_pfnHook;*/
		TinyString			m_calleeModName;

	};
}
