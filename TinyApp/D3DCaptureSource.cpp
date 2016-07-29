#include "stdafx.h"
#include "D3DCaptureSource.h"
#include "D3DUtility.h"
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

namespace D3D
{
	void RoundVector2D(D3DXVECTOR2 &v)
	{
		v.x = float(round(v.x));
		v.y = float(round(v.y));
	}
	//////////////////////////////////////////////////////////////////////////
	CD3DCaptureSource::CD3DCaptureSource(CD3D10Device& system)
		:m_bCapturing(FALSE),
		m_device(system),
		m_pSharedCapture(NULL)
	{
		::ZeroMemory(&m_targetWND, sizeof(m_targetWND));
	}


	CD3DCaptureSource::~CD3DCaptureSource()
	{
		if (m_targetWND.hProcess)
		{
			CloseHandle(m_targetWND.hProcess);
		}
	}

	BOOL CD3DCaptureSource::Initialize(const TinyString& processName)
	{
		if (!FindWindow(processName))
			return FALSE;
		stringstream stream;
		stream << BEGIN_CAPTURE_EVENT << m_targetWND.dwProcessID;
		if (!m_eventBegin.CreateEvent(FALSE, FALSE, stream.str().c_str()))
		{
			if (!m_eventBegin.OpenEvent(EVENT_ALL_ACCESS, FALSE, stream.str().c_str()))
			{
				return FALSE;
			}
		}
		stream.str("");
		stream << END_CAPTURE_EVENT << m_targetWND.dwProcessID;
		if (!m_eventEnd.CreateEvent(FALSE, FALSE, stream.str().c_str()))
		{
			if (!m_eventEnd.OpenEvent(EVENT_ALL_ACCESS, FALSE, stream.str().c_str()))
			{
				return FALSE;
			}
		}
		stream.str("");
		stream << CAPTURE_READY_EVENT << m_targetWND.dwProcessID;
		if (!m_eventReady.CreateEvent(FALSE, FALSE, stream.str().c_str()))
		{
			if (!m_eventReady.OpenEvent(EVENT_ALL_ACCESS, FALSE, stream.str().c_str()))
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	BOOL CD3DCaptureSource::BeginCapture(const TinyString& processName)
	{
		BOOL bRes = S_OK;
		if (!Initialize(processName))
			return FALSE;
		if (!D3D::InjectLibrary(m_targetWND.hProcess, TEXT("D:\\Develop\\GitHub\\TinyUI\\Debug\\D3DCaptureHook.dll")))
			return FALSE;
		m_pSharedCapture = GetSharedCapture();
		if (!m_pSharedCapture)
			return FALSE;
		if (!m_textureCapture.Initialize(&m_device, m_pSharedCapture))
			return FALSE;
		m_gameTexture.CreateTexture(&m_device, m_pSharedCapture->Size, (DXGI_FORMAT)m_pSharedCapture->Format, 0, FALSE, TRUE);
		m_bCapturing = TRUE;
		m_eventBegin.SetEvent();
		return TRUE;
	}

	BOOL CD3DCaptureSource::EndCapture()
	{
		m_bCapturing = FALSE;
		m_eventEnd.SetEvent();
		return TRUE;
	}

	SharedCapture* CD3DCaptureSource::GetSharedCapture()
	{
		if (m_sharedCapture.Open(SHAREDCAPTURE) && m_sharedCapture.Map())
		{
			return reinterpret_cast<SharedCapture*>(m_sharedCapture.Address());
		}
		return NULL;
	}
	void CD3DCaptureSource::Tick(FLOAT fSeconds)
	{
		if (m_eventExit.Lock(0))
		{
			EndCapture();
		}
		if (!m_bCapturing)
		{
			BeginCapture(TEXT("War3.exe"));
		}
		else
		{

		}
	}
	BOOL CD3DCaptureSource::Render(const D3DXVECTOR2 &pos, const D3DXVECTOR2 &size)
	{
		if (!m_pSharedCapture || !m_gameTexture.IsValid())
			return FALSE;

		CD3D10Texture *texture = m_textureCapture.LockTexture(&m_device);
		if (texture)
		{
			TinySize gameSize = m_gameTexture.GetSize();
			D3DXVECTOR2 texturePos = D3DXVECTOR2(0.0F, 0.0F);
			D3DXVECTOR2 textureStretch = D3DXVECTOR2(1.0F, 1.0F);
			D3DXVECTOR2 textureSize = D3DXVECTOR2(FLOAT(gameSize.cx), FLOAT(gameSize.cy));
			gameSize.cx = GetSystemMetrics(SM_CXSCREEN);
			gameSize.cy = GetSystemMetrics(SM_CYSCREEN);
			D3DXVECTOR2 totalSize = D3DXVECTOR2(FLOAT(gameSize.cx), FLOAT(gameSize.cy));
			D3DXVECTOR2 center = totalSize*0.5F;
			FLOAT xAspect = totalSize.x / textureSize.x;
			FLOAT yAspect = totalSize.y / textureSize.y;
			FLOAT multiplyVal = ((textureSize.y * xAspect) > totalSize.y) ? yAspect : xAspect;
			textureSize *= multiplyVal;
			textureStretch = Multiply(textureStretch, textureSize);
			texturePos = center - (textureStretch*0.5F);
			D3DXVECTOR2 sizeAdjust = Divide(size, totalSize);
			texturePos = Multiply(texturePos, sizeAdjust);
			texturePos += pos;
			textureStretch = Multiply(textureStretch, sizeAdjust);
			RoundVector2D(texturePos);
			RoundVector2D(textureSize);
			if (m_pSharedCapture->bFlip)
				DrawSprite(texture, 0xFFFFFFFF, texturePos.x, texturePos.y + textureStretch.y, texturePos.x + textureStretch.x, texturePos.y);
			else
				DrawSprite(texture, 0xFFFFFFFF, texturePos.x, texturePos.y, texturePos.x + textureStretch.x, texturePos.y + textureStretch.y);
		}
		return TRUE;
	}
	void CD3DCaptureSource::DrawSprite(CD3D10Texture *texture, DWORD color, float x, float y, float x2, float y2)
	{
		m_device.DrawSprite(texture, color, x, y, x2, y2, 0.0F, 0.0F, 1.0F, 1.0F);
	}

	BOOL CALLBACK CD3DCaptureSource::EnumWindow(HWND hwnd, LPARAM lParam)
	{
		LPWNDINFO ws = reinterpret_cast<LPWNDINFO>(lParam);
		DWORD dwProcessId;
		DWORD dwThreadID = GetWindowThreadProcessId(hwnd, &dwProcessId);
		if (dwProcessId == ws->dwProcessID)
		{
			ws->hWND = hwnd;
			HWND hWND = ::GetParent(hwnd);
			while (hWND)
			{
				ws->hWND = hWND;
				ws->dwThreadID = dwThreadID;
				hWND = ::GetParent(hWND);
			}
			return FALSE;
		}
		return TRUE;
	}
	BOOL CD3DCaptureSource::FindWindow(const TinyString& processName)
	{
		if (m_targetWND.hProcess)
			CloseHandle(m_targetWND.hProcess);
		BOOL bRes = S_OK;
		m_targetWND.dwProcessID = FindProcess(processName);
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_targetWND.dwProcessID);
		if (!hProcess)
		{
			bRes = FALSE;
			goto D3DERROR;
		}
		if (!DuplicateHandle(GetCurrentProcess(), hProcess, GetCurrentProcess(), &m_targetWND.hProcess, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			bRes = FALSE;
			goto D3DERROR;
		}
		WNDINFO ws;
		ws.dwProcessID = m_targetWND.dwProcessID;
		bRes = EnumWindows(CD3DCaptureSource::EnumWindow, reinterpret_cast<LPARAM>(&ws));
		m_targetWND.dwThreadID = ws.dwThreadID;
		m_targetWND.hWND = ws.hWND;
	D3DERROR:
		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
		return bRes;
	}
	DWORD CD3DCaptureSource::FindProcess(const TinyString& processName)
	{
		DWORD dwProcessID = 0;
		HANDLE hProcessSnap = NULL;
		PROCESSENTRY32 pe32 = { 0 };
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == (HANDLE)-1)
			return FALSE;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				TinyString szExeFile(pe32.szExeFile);
				if (szExeFile == processName)
				{
					dwProcessID = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}
		CloseHandle(hProcessSnap);
		return dwProcessID;
	}
}
