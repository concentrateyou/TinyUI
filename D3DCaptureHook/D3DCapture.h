#pragma once
#include "D3DHook.h"
#include <string>
using namespace std;

/// <summary>
/// 3DÍ¼Ïñ²¶»ñ
/// </summary>
class D3DCapture
{
public:
	D3DCapture();
	~D3DCapture();
private:
	string m_strKeepAlive;
};
