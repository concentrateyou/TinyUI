// D3DHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "D3DDetour.h"

CD3DDetour::CD3DDetour()
	:m_bDetour(FALSE)
{

}
CD3DDetour::~CD3DDetour()
{

}
BOOL CD3DDetour::Initialize(FARPROC pfnOrig, FARPROC pfnNew)
{
	if (!pfnOrig || !pfnNew)
		return FALSE;
	m_pfnOrig = pfnOrig;
	m_pfnNew = pfnNew;
	if (!VirtualProtect((LPVOID)m_pfnOrig, 5, PAGE_EXECUTE_READWRITE, &m_dwOrigProtect))
		return FALSE;
	memcpy(m_data, (const void*)m_pfnOrig, 5);
	return TRUE;
}
BOOL CD3DDetour::BeginDetour()
{
	if (!m_pfnOrig || !m_pfnNew)
		return FALSE;
	ULONG start = ULONG(m_pfnOrig);
	ULONG target = ULONG(m_pfnNew);
	ULONG64 offset = 0;
	ULONG64 diff = 0;
	//http://www.cnblogs.com/zhangdongsheng/archive/2012/12/06/2804234.html
	//计算偏移量(JMP的地址–代码地址–5 = 机器码跳转地址 x86)
	offset = target - (start + 5);
	DWORD oldProtect;
	VirtualProtect((LPVOID)m_pfnOrig, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	LPBYTE ps = (LPBYTE)m_pfnOrig;
	*ps = 0xE9;
	*(DWORD*)(ps + 1) = DWORD(offset);
	m_bDetour = TRUE;
	return TRUE;
}
BOOL CD3DDetour::EndDetour()
{
	if (!m_bDetour) return FALSE;
	DWORD oldProtect;
	VirtualProtect((LPVOID)m_pfnOrig, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((void*)m_pfnOrig, m_data, 5);
	VirtualProtect((LPVOID)m_pfnOrig, 5, m_dwOrigProtect, &oldProtect);
	m_bDetour = FALSE;
	return TRUE;
}
BOOL CD3DDetour::IsValid() const
{
	return m_bDetour;
}
FARPROC CD3DDetour::GetOrig() const
{
	return m_pfnOrig;
}
//////////////////////////////////////////////////////////////////////////

