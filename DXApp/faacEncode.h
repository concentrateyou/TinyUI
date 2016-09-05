#pragma once
#include "RTMPPublisher.h"
extern "C"
{
#include "faaccfg.h"
#include "faac.h"
}
#pragma comment(lib,"libfaac.lib")

class RTMPPublisher;
/// <summary>
/// “Ù∆µ±‡¬Î
/// </summary>
class faacEncode
{
public:
	faacEncode();
	~faacEncode();
	BOOL Open();
	BOOL Encode();
	BOOL Close();
private:
	faacEncHandle m_handle;
};

