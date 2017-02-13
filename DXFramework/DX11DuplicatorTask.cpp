#include "stdafx.h"
#include "DX11DuplicatorTask.h"

namespace DXFramework
{
	DX11DuplicatorTask::DX11DuplicatorTask(DX11* pDX11, DX11Image& image)
		:m_bCapturing(FALSE),
		m_pDX11(pDX11),
		m_image(image)
	{
		ZeroMemory(&m_targetProcess, sizeof(m_targetProcess));
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	}
	DX11DuplicatorTask::~DX11DuplicatorTask()
	{

	}
	void DX11DuplicatorTask::SetConfig(const TinyString& exeName, const TinyString& dllName)
	{
		m_exeName = std::move(exeName);
		m_dllName = std::move(dllName);
	}
	BOOL DX11DuplicatorTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&DX11DuplicatorTask::OnMessagePump, this));
	}
	BOOL DX11DuplicatorTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}
	BOOL DX11DuplicatorTask::OpenEvents()
	{
		string name = StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, m_targetProcess.dwProcessID);
		if (!m_start.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetProcess.dwProcessID));
		if (!m_stop.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetProcess.dwProcessID));
		if (!m_ready.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_EXIT_EVENT, m_targetProcess.dwProcessID));
		if (!m_exit.OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str()))
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL DX11DuplicatorTask::CreateEvents()
	{
		string name = StringPrintf("%s%d", BEGIN_CAPTURE_EVENT, m_targetProcess.dwProcessID);
		if (!m_start.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", END_CAPTURE_EVENT, m_targetProcess.dwProcessID));
		if (!m_stop.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_READY_EVENT, m_targetProcess.dwProcessID));
		if (!m_ready.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		name = std::move(StringPrintf("%s%d", CAPTURE_EXIT_EVENT, m_targetProcess.dwProcessID));
		if (!m_exit.CreateEvent(FALSE, FALSE, name.c_str()))
		{
			return FALSE;
		}
		return TRUE;
	}
	void DX11DuplicatorTask::CloseEvents()
	{
		m_start.Close();
		m_stop.Close();
		m_ready.Close();
		m_exit.Close();
	}
	SharedCaptureDATA* DX11DuplicatorTask::GetSharedCaptureDATA()
	{
		if (!m_captureMemory.Address())
		{
			if (!m_captureMemory.Open(SHAREDCAPTURE_MEMORY, FALSE))
				return NULL;
			if (!m_captureMemory.Map(0, sizeof(SharedCaptureDATA)))
				return NULL;
		}
		SharedCaptureDATA* pDATA = reinterpret_cast<SharedCaptureDATA*>(m_captureMemory.Address());
		return pDATA;
	}
	SharedTextureDATA* DX11DuplicatorTask::GetSharedTextureDATA(DWORD dwSize)
	{
		if (m_textureMemery.GetSize() != dwSize)
		{
			m_textureMemery.Unmap();
			m_textureMemery.Close();
		}
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
				return NULL;
			if (!m_textureMemery.Map(0, dwSize))
				return NULL;
		}
		return reinterpret_cast<SharedTextureDATA*>(m_textureMemery.Address());
	}
	BYTE*	DX11DuplicatorTask::GetSharedTexture(DWORD dwSize)
	{
		if (m_textureMemery.GetSize() != dwSize)
		{
			m_textureMemery.Unmap();
			m_textureMemery.Close();
		}
		if (!m_textureMemery.Address())
		{
			if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
				return NULL;
			if (!m_textureMemery.Map(0, dwSize))
				return NULL;
		}
		return reinterpret_cast<LPBYTE>(m_textureMemery.Address());
	}
	BOOL DX11DuplicatorTask::BeginCapture()
	{
		ASSERT(m_pDX11);
		BOOL bRes = S_OK;
		if (!CreateEvents())
		{
			TRACE("BeginCapture BuildEvents-FAIL\n");
			return FALSE;
		}
		SharedCaptureDATA* pCaptureDATA = GetSharedCaptureDATA();
		if (!pCaptureDATA)
		{
			TRACE("BeginCapture GetSharedCaptureDATA-FAIL\n");
			return FALSE;
		}
		SharedTextureDATA* pTextureDATA = GetSharedTextureDATA(pCaptureDATA->MapSize);
		if (!pTextureDATA)
		{
			TRACE("BeginCapture GetSharedTextureDATA-FAIL\n");
			return FALSE;
		}
		do
		{
			if (pCaptureDATA->CaptureType == CAPTURETYPE_SHAREDTEXTURE)
			{
				if (!pTextureDATA->TextureHandle)
				{
					TRACE("BeginCapture GetSharedTextureDATA-TextureHandle==NULL-FAIL\n");
					return FALSE;
				}
				TinyAutoLock lock(*m_pDX11);
				m_image.Destory();
				if (!m_image.Load(*m_pDX11, pTextureDATA->TextureHandle))
				{
					TRACE("BeginCapture m_image.Load-FAIL\n");
					return FALSE;
				}
				break;
			}
			if (pCaptureDATA->CaptureType == CAPTURETYPE_MEMORYTEXTURE)
			{
				TinyAutoLock lock(*m_pDX11);
				m_image.Destory();
				if (!m_image.Create(*m_pDX11, pCaptureDATA->Size, NULL, FALSE))
				{
					TRACE("BeginCapture m_image.Create-FAIL\n");
					return FALSE;
				}
				break;
			}
		} while (0);
		return TRUE;
	}
	BOOL DX11DuplicatorTask::EndCapture()
	{
		TRACE("EndCapture\n");
		m_start.Close();
		m_stop.Close();
		m_ready.Close();
		m_exit.Close();
		if (m_targetProcess.hProcess)
		{
			CloseHandle(m_targetProcess.hProcess);
			m_targetProcess.hProcess = NULL;
		}
		ZeroMemory(&m_targetProcess, sizeof(m_targetProcess));
		m_textureMemery.Unmap();;
		m_textureMemery.Close();
		m_captureMemory.Unmap();
		m_captureMemory.Close();
		m_bCapturing = FALSE;
		m_image.Destory();
		return TRUE;
	}
	BOOL DX11DuplicatorTask::AttemptCapture(const TinyString& exeName, const TinyString& dllName)
	{
		HANDLE hProcess = NULL;
		if (!ProcessExists(exeName, m_targetProcess))
		{
			m_bCapturing = FALSE;
			goto _ERROR;
		}
		if (OpenEvents())
		{
			m_start.SetEvent();
			m_bCapturing = TRUE;
			return m_bCapturing;
		}
		else
		{
			if (!CreateEvents())
			{
				TRACE("BuildEvents == FALSE\n");
				goto _ERROR;
			}
		}

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_targetProcess.dwProcessID);
		if (!hProcess)
		{
			TRACE("hProcess == NULL\n");
			CloseEvents();
			goto _ERROR;
		}
		TRACE("Begin InjectLibrary\n");
		if (!InjectLibrary(hProcess, dllName.STR()))
		{
			TRACE("InjectLibrary - FALSE\n");
			CloseEvents();
			goto _ERROR;
		}
		if (!DuplicateHandle(GetCurrentProcess(), hProcess, GetCurrentProcess(), &m_targetProcess.hProcess, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			TRACE("DuplicateHandle - FALSE\n");
			CloseEvents();
			goto _ERROR;
		}
		m_start.SetEvent();
		Sleep(500);
		m_bCapturing = BeginCapture();
		if (m_bCapturing)
		{
			TRACE("AttemptCapture - m_bCapturing = TRUE\n");
		}
	_ERROR:
		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
		return m_bCapturing;
	}
	void DX11DuplicatorTask::Tick()
	{
		if (m_exit && m_exit.Lock(0))
		{
			TRACE("Tick - EndCapture\n");
			EndCapture();
		}
		if (m_bCapturing && !m_ready && m_targetProcess.dwProcessID)
		{
			TRACE("Tick - CreateEvents\n");
			CreateEvents();
		}
		if (m_ready && m_ready.Lock(0))
		{
			TRACE("Tick - BeginCapture\n");
			BeginCapture();
		}
		if (!m_bCapturing)
		{
			AttemptCapture(m_exeName, m_dllName);
		}
		else
		{
			if (m_targetProcess.hProcess && WaitForSingleObject(m_targetProcess.hProcess, 0) == WAIT_OBJECT_0)
			{
				TRACE("m_targetWND.hProcess && WaitForSingleObject(m_targetWND.hProcess, 0) == WAIT_OBJECT_0\n");
				EndCapture();
			}
		}
	}
	void DX11DuplicatorTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(5))
			{
				if (m_stop)
				{
					m_stop.SetEvent();
				}
				EndCapture();
				break;
			}
			Tick();
		}
	}
	PROCESSINFO	DX11DuplicatorTask::GetPROCESSINFO()
	{
		return m_targetProcess;
	}
}
