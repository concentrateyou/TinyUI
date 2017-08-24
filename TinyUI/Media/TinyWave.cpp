#include "../stdafx.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		TinyWaveOut::TinyWaveOut()
			:m_hWAVE(NULL)
		{
		}
		TinyWaveOut::~TinyWaveOut()
		{
			Close();
		}
		TinyWaveOut::operator HWAVEOUT() const throw()
		{
			return (m_hWAVE);
		}
		BOOL TinyWaveOut::QueryFormat(LPWAVEFORMATEX pwfx)
		{
			ASSERT(pwfx != NULL);
			return waveOutOpen(NULL, WAVE_MAPPER, pwfx, NULL, 0, WAVE_FORMAT_QUERY) == MMSYSERR_NOERROR;
		}
		BOOL TinyWaveOut::Open(LPWAVEFORMATEX pwfx, DWORD_PTR dwCallbackInstance)
		{
			ASSERT(pwfx != NULL);
			if (QueryFormat(pwfx) != MMSYSERR_NOERROR)
				return FALSE;
			return waveOutOpen(&m_hWAVE, WAVE_MAPPER, pwfx, (DWORD_PTR)TinyWaveOut::waveOutProc, (DWORD_PTR)dwCallbackInstance, CALLBACK_FUNCTION) == MMSYSERR_NOERROR;
		}
		void TinyWaveOut::GetErrorText(LPTSTR pzText, MMRESULT hRes)
		{
			waveOutGetErrorText(hRes, pzText, MAX_PATH);
		}
		BOOL TinyWaveOut::Pause()
		{
			if (waveOutPause(m_hWAVE) != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveOut::Reset()
		{
			MMRESULT hRes = waveOutReset(m_hWAVE);
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveOut::Restart()
		{
			MMRESULT hRes = waveOutRestart(m_hWAVE);
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveOut::Close()
		{
			return waveOutClose(m_hWAVE) == MMSYSERR_NOERROR;
		}
		BOOL TinyWaveOut::SetVolume(DWORD dwVolume)
		{
			MMRESULT hRes = waveOutSetVolume(m_hWAVE, dwVolume);
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveOut::GetDevCaps(UINT_PTR uDeviceID, LPWAVEOUTCAPS pwoc)
		{
			MMRESULT hRes = waveOutGetDevCaps(uDeviceID, pwoc, sizeof(WAVEOUTCAPS));
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		DWORD TinyWaveOut::GetVolume()
		{
			DWORD dwVolume;
			if (m_hWAVE != NULL)
			{
				waveOutGetVolume(m_hWAVE, &dwVolume);
			}
			return dwVolume;
		}
		BOOL TinyWaveOut::GetPosition(LPMMTIME pmmt)
		{
			MMRESULT hRes = waveOutGetPosition(m_hWAVE, pmmt, sizeof(WAVEOUTCAPS));
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		DWORD TinyWaveOut::GetNumDevs()
		{
			return waveOutGetNumDevs();
		}
		BOOL TinyWaveOut::Prepare(LPWAVEHDR pwh)
		{
			MMRESULT hRes = waveOutPrepareHeader(m_hWAVE, pwh, sizeof(WAVEOUTCAPS));
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveOut::Unprepare(LPWAVEHDR pwh)
		{
			MMRESULT hRes = waveOutUnprepareHeader(m_hWAVE, pwh, sizeof(WAVEOUTCAPS));
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveOut::Write(LPWAVEHDR pwh)
		{
			MMRESULT hRes = waveOutWrite(m_hWAVE, pwh, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveOut::SetPlaybackRate(DWORD dwRate)
		{
			MMRESULT hRes = waveOutSetPlaybackRate(m_hWAVE, dwRate);
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveOut::BreakLoop()
		{
			MMRESULT hRes = waveOutBreakLoop(m_hWAVE);
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		void TinyWaveOut::SendMessage(UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
		{
			waveOutMessage(m_hWAVE, uMsg, dwParam1, dwParam2);
		}
		void CALLBACK TinyWaveOut::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
		{
			TinyWaveOut* waveOut = (TinyWaveOut*)dwInstance;
			switch (uMsg)
			{
			case MM_WOM_OPEN:
				waveOut->OnOpen();
				break;
			case MM_WOM_CLOSE:
				waveOut->OnClose();
				break;
			case MM_WOM_DONE:
				waveOut->OnDone((LPWAVEHDR)dwParam1, dwInstance);
				break;
			}
		}
		void TinyWaveOut::OnDone(LPWAVEHDR wp, DWORD_PTR dwInstance)
		{
			WaveDone(wp, dwInstance);
		}
		void TinyWaveOut::OnOpen()
		{
		}
		void TinyWaveOut::OnClose()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		TinyWaveIn::TinyWaveIn()
			:m_hWAVE(NULL)
		{

		}
		TinyWaveIn::~TinyWaveIn()
		{
			Close();
		}
		BOOL TinyWaveIn::QueryFormat(LPWAVEFORMATEX pwfx)
		{
			ASSERT(pwfx != NULL);
			return waveInOpen(NULL, WAVE_MAPPER, pwfx, NULL, 0, WAVE_FORMAT_QUERY) == MMSYSERR_NOERROR;
		}
		TinyWaveIn::operator HWAVEIN() const throw()
		{
			return (m_hWAVE);
		}
		BOOL TinyWaveIn::Open(LPWAVEFORMATEX pwfx, DWORD_PTR dwCallbackInstance)
		{
			ASSERT(pwfx != NULL);
			if (QueryFormat(pwfx) != MMSYSERR_NOERROR)
				return FALSE;
			return waveInOpen(&m_hWAVE, WAVE_MAPPER, pwfx, (DWORD_PTR)TinyWaveIn::waveInProc, (DWORD_PTR)dwCallbackInstance, CALLBACK_FUNCTION);
		}
		BOOL TinyWaveIn::Close()
		{
			return waveInClose(m_hWAVE) == MMSYSERR_NOERROR;
		}
		BOOL TinyWaveIn::Start()
		{
			MMRESULT hRes = waveInStart(m_hWAVE);
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveIn::Stop()
		{
			MMRESULT hRes = waveInStop(m_hWAVE);
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveIn::Reset()
		{
			MMRESULT hRes = waveInReset(m_hWAVE);
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveIn::Prepare(LPWAVEHDR pwh)
		{
			MMRESULT hRes = waveInPrepareHeader(m_hWAVE, pwh, sizeof(WAVEOUTCAPS));
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveIn::Unprepare(LPWAVEHDR pwh)
		{
			MMRESULT hRes = waveInUnprepareHeader(m_hWAVE, pwh, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveIn::Add(LPWAVEHDR pwh)
		{
			MMRESULT hRes = waveInAddBuffer(m_hWAVE, pwh, sizeof(WAVEHDR));
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyWaveIn::GetPosition(LPMMTIME pmmt)
		{
			MMRESULT hRes = waveInGetPosition(m_hWAVE, pmmt, sizeof(WAVEINCAPS));
			if (hRes != MMSYSERR_NOERROR)
			{
				Close();
				return FALSE;
			}
			return TRUE;
		}
		void TinyWaveIn::GetErrorText(LPTSTR pzText, MMRESULT hRes)
		{
			waveInGetErrorText(hRes, pzText, MAX_PATH);
		}
		void TinyWaveIn::OnOpen()
		{

		}
		void TinyWaveIn::OnClose()
		{

		}
		void TinyWaveIn::OnData(LPWAVEHDR wp, DWORD_PTR dwInstance)
		{

		}
		void CALLBACK TinyWaveIn::waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
		{
			TinyWaveIn* waveIn = (TinyWaveIn*)dwInstance;
			switch (uMsg)
			{
			case MM_WIM_OPEN:
				waveIn->OnOpen();
				break;
			case MM_WIM_CLOSE:
				waveIn->OnClose();
				break;
			case MM_WIM_DATA:
				waveIn->OnData((LPWAVEHDR)dwParam1, dwInstance);
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWavePlayer::TinyWavePlayer()
		{
			m_wavePlayDone.Reset(new Delegate<void(LPWAVEHDR, DWORD_PTR)>(this, &TinyWavePlayer::PlayDone));
			m_waveOut.WaveDone += m_wavePlayDone;
			m_dwBlocks = m_dwBlockNext = m_dwBlockOut = 0;
			m_bPlaying = FALSE;
		}
		TinyWavePlayer::~TinyWavePlayer()
		{
			m_waveOut.WaveDone -= m_wavePlayDone;
			while (m_dwBlocks > 0)
			{
				--m_dwBlocks;
				m_waveOut.Unprepare(m_audioBlockPtr[m_dwBlocks]);
				LocalFree((LPBYTE)m_audioBlockPtr[m_dwBlocks]);
			}
			m_waveOut.Close();
			m_waveFile.Close();
			m_dwBlockNext = m_dwBlockOut = 0;
			m_bPlaying = FALSE;
		}
		BOOL TinyWavePlayer::LoadStream(LPVOID pStream, DWORD bufferSize)
		{
			m_waveFile.Close();
			if (!m_waveFile.Open(pStream, bufferSize)) return FALSE;
			WAVEFORMATEX* waveEx = m_waveFile.GetFormat();
			if (m_waveOut.Open(waveEx, (DWORD_PTR)&m_waveOut) != MMSYSERR_NOERROR)
				return FALSE;
			for (m_dwBlocks = 0; m_dwBlocks < MAX_AUDIO_BUFFERS; m_dwBlocks++)
			{
				if (!(m_audioBlockPtr[m_dwBlocks] = (LPWAVEHDR)LocalAlloc(LPTR, (DWORD)(sizeof(WAVEHDR) + AUDIO_BUFFER_SIZE))))
					break;
				m_audioBlockPtr[m_dwBlocks]->dwFlags = WHDR_DONE;
				m_audioBlockPtr[m_dwBlocks]->lpData = (LPSTR)((LPBYTE)m_audioBlockPtr[m_dwBlocks] + sizeof(WAVEHDR));
				m_audioBlockPtr[m_dwBlocks]->dwBufferLength = AUDIO_BUFFER_SIZE;
				if (m_waveOut.Prepare(m_audioBlockPtr[m_dwBlocks]) != MMSYSERR_NOERROR)
				{
					LocalFree(m_audioBlockPtr[m_dwBlocks]);
					continue;
				}
			}
			if (m_dwBlocks < MIN_AUDIO_BUFFERS)
			{
				while (m_dwBlocks > 0)
				{
					--m_dwBlocks;
					m_waveOut.Unprepare(m_audioBlockPtr[m_dwBlocks]);
					LocalFree((LPBYTE)m_audioBlockPtr[m_dwBlocks]);
				}
				m_waveOut.Close();
				m_waveFile.Close();
			}
			return TRUE;
		}
		BOOL TinyWavePlayer::LoadFile(LPSTR szFilename)
		{
			m_waveFile.Close();
			if (!m_waveFile.Open(szFilename)) return FALSE;
			WAVEFORMATEX* waveEx = m_waveFile.GetFormat();
			if (m_waveOut.Open(waveEx, (DWORD_PTR)&m_waveOut) != MMSYSERR_NOERROR)
				return FALSE;
			for (m_dwBlocks = 0; m_dwBlocks < MAX_AUDIO_BUFFERS; m_dwBlocks++)
			{
				if (!(m_audioBlockPtr[m_dwBlocks] = (LPWAVEHDR)LocalAlloc(LPTR, (DWORD)(sizeof(WAVEHDR) + AUDIO_BUFFER_SIZE))))
					break;
				m_audioBlockPtr[m_dwBlocks]->dwFlags = WHDR_DONE;
				m_audioBlockPtr[m_dwBlocks]->lpData = (LPSTR)((LPBYTE)m_audioBlockPtr[m_dwBlocks] + sizeof(WAVEHDR));
				m_audioBlockPtr[m_dwBlocks]->dwBufferLength = AUDIO_BUFFER_SIZE;
				if (m_waveOut.Prepare(m_audioBlockPtr[m_dwBlocks]) != MMSYSERR_NOERROR)
				{
					LocalFree(m_audioBlockPtr[m_dwBlocks]);
					continue;
				}
			}
			if (m_dwBlocks < MIN_AUDIO_BUFFERS)
			{
				while (m_dwBlocks > 0)
				{
					--m_dwBlocks;
					m_waveOut.Unprepare(m_audioBlockPtr[m_dwBlocks]);
					LocalFree((LPBYTE)m_audioBlockPtr[m_dwBlocks]);
				}
				m_waveOut.Close();
				m_waveFile.Close();
			}
			return FALSE;
		}
		BOOL TinyWavePlayer::IsPlaying()
		{
			BOOL bFlags = FALSE;
			for (DWORD i = 0; i < m_dwBlocks; i++)
			{
				if ((m_audioBlockPtr[i]->dwFlags & WHDR_DONE) == 0)//正在播放
				{
					bFlags = TRUE;
				}
			}
			return bFlags;
		}
		BOOL TinyWavePlayer::Seek(DWORD newPos)
		{
			newPos += m_waveFile.GetDataOffset();
			return m_waveFile.Seek(newPos, SEEK_SET);
		}
		BOOL TinyWavePlayer::Pause()
		{
			m_bPlaying = FALSE;
			return m_waveOut.Pause();
		}
		BOOL TinyWavePlayer::Restart()
		{
			m_bPlaying = TRUE;
			return m_waveOut.Restart();
		}
		DWORD TinyWavePlayer::GetDataSize()
		{
			return m_waveFile.GetSize();
		}
		void TinyWavePlayer::PlayData()
		{
			if (!m_bPlaying) return;
			while (m_dwBlockOut < m_dwBlocks)
			{
				LONG numberOfBytesRead = 0;
				m_waveFile.Read((LPBYTE)m_audioBlockPtr[m_dwBlockNext]->lpData, min(AUDIO_BUFFER_SIZE, m_dwSizeL), &numberOfBytesRead);
				m_dwSizeL -= numberOfBytesRead;
				if (numberOfBytesRead > 0)
				{
					m_audioBlockPtr[m_dwBlockNext]->dwBytesRecorded = numberOfBytesRead;
					m_waveOut.Write(m_audioBlockPtr[m_dwBlockNext]);
					if (m_dwSizeL <= 0)
					{
						++m_dwBlockOut;
						++m_dwBlockNext;
					}
				}
				if (m_dwSizeL <= 0) break;
				++m_dwBlockOut;
				++m_dwBlockNext;
				if (m_dwBlockNext >= m_dwBlocks)
				{
					m_dwBlockNext = 0;
				}
			}
		}
		BOOL TinyWavePlayer::Reset()
		{
			return m_waveOut.Reset();
		}
		BOOL TinyWavePlayer::Play(BOOL bLoop)
		{
			do
			{
				m_bPlaying = TRUE;
				m_dwBlockOut = m_dwBlockNext = 0;
				m_dwSizeL = m_waveFile.GetSize();
				PlayData();//播放数据
				if (m_waveOut.Restart() != MMSYSERR_NOERROR)
					return FALSE;
				while (m_dwBlockOut > 0)
				{
					MSG msg = { 0 };
					if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) && (msg.message == PLAY_QUIT))
					{
						//收到窗口退出消息
						m_bPlaying = FALSE;
						do
						{
							LoopEmpty();
						} while (IsPlaying());
						m_waveOut.Reset();
						while (m_dwBlocks > 0)
						{
							--m_dwBlocks;
							m_waveOut.Unprepare(m_audioBlockPtr[m_dwBlocks]);
							LocalFree((LPBYTE)m_audioBlockPtr[m_dwBlocks]);
						}
						m_waveOut.Close();
						m_waveFile.Close();
						break;
					}
				}
			} while (bLoop);
			return TRUE;
		}
		void TinyWavePlayer::PlayDone(LPWAVEHDR wp, DWORD_PTR dwInstance)
		{
			if (wp->dwFlags & WHDR_DONE)
			{
				--m_dwBlockOut;
				PlayData();
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWaveTape::TinyWaveTape()
		{

		}
		TinyWaveTape::~TinyWaveTape()
		{

		}
		BOOL TinyWaveTape::LoadFile(LPTSTR pzFile)
		{
			return FALSE;
		}
		BOOL TinyWaveTape::Start()
		{
			return FALSE;
		}
		BOOL TinyWaveTape::Stop()
		{
			return FALSE;
		}
		BOOL TinyWaveTape::Reset()
		{
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		//Wave File http://msdn.microsoft.com/en-us/library/windows/desktop/dd742884(v=vs.85).aspx
		TinyWaveFile::TinyWaveFile()
			:m_hmmio(NULL),
			m_reading(0)
		{
			ZeroMemory(&m_mmckRIFF, sizeof(m_mmckRIFF));
			ZeroMemory(&m_mmckFMT, sizeof(m_mmckFMT));
			ZeroMemory(&m_mmckFACT, sizeof(m_mmckFACT));
			ZeroMemory(&m_mmckDATA, sizeof(m_mmckDATA));
		}
		TinyWaveFile::~TinyWaveFile()
		{
			Close();
		}
		TinyWaveFile::operator HMMIO() const throw()
		{
			return m_hmmio;
		}
		BOOL TinyWaveFile::Create(LPTSTR pzFile, const WAVEFORMATEX* pWaveEx)
		{
			if (!pzFile)
				return FALSE;
			if (pWaveEx->wFormatTag == WAVE_FORMAT_PCM)
			{
				m_waveEx.Reset(new BYTE[sizeof(WAVEFORMATEX)]);
				memcpy((void*)m_waveEx, (void*)pWaveEx, sizeof(WAVEFORMATEX));
			}
			else
			{
				m_waveEx.Reset(new BYTE[sizeof(WAVEFORMATEX) + pWaveEx->cbSize]);
				memcpy((void*)m_waveEx, (void*)pWaveEx, sizeof(WAVEFORMATEX) + pWaveEx->cbSize);
			}
			if (m_hmmio != NULL)
			{
				mmioClose(m_hmmio, 0);
				m_hmmio = NULL;
			}
			if ((m_hmmio = mmioOpen(pzFile, NULL, MMIO_CREATE | MMIO_READWRITE | MMIO_EXCLUSIVE | MMIO_ALLOCBUF)) == NULL)
				return FALSE;
			m_reading = FALSE;
			MMRESULT  mmRes = MMSYSERR_NOERROR;
			m_mmckRIFF.ckid = mmioRIFF;
			m_mmckRIFF.fccType = mmioWAVE;
			mmRes = mmioCreateChunk(m_hmmio, &m_mmckRIFF, MMIO_CREATERIFF);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			m_mmckFMT.ckid = mmioFMT;
			mmRes = mmioCreateChunk(m_hmmio, &m_mmckFMT, 0);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			DWORD dwSize = sizeof(WAVEFORMATEX) + pWaveEx->cbSize;
			if (dwSize != mmioWrite(m_hmmio, (const char*)pWaveEx, dwSize))
				goto MMIO_ERROR;
			mmRes = mmioAscend(m_hmmio, &m_mmckFMT, 0);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			m_mmckFACT.ckid = mmioFACT;
			mmRes = mmioCreateChunk(m_hmmio, &m_mmckFACT, 0);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			DWORD dwSamples = 0;
			dwSize = sizeof(DWORD);
			if (dwSize != mmioWrite(m_hmmio, (HPSTR)&dwSamples, dwSize))
				goto MMIO_ERROR;
			mmRes = mmioAscend(m_hmmio, &m_mmckFACT, 0);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			m_mmckDATA.ckid = mmioDATA;
			mmRes = mmioCreateChunk(m_hmmio, &m_mmckDATA, 0);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			return TRUE;
		MMIO_ERROR:
			mmioClose(m_hmmio, 0);
			m_hmmio = NULL;
			return FALSE;
		}
		BOOL TinyWaveFile::Write(BYTE* lpBuffer, LONG nNumberOfBytesToRead)
		{
			if (!m_hmmio)
				return FALSE;
			MMRESULT  mmRes = MMSYSERR_NOERROR;
			mmRes = mmioWrite(m_hmmio, (const char*)lpBuffer, nNumberOfBytesToRead);
			return mmRes != MMSYSERR_NOERROR;
		}
		BOOL TinyWaveFile::Open(LPTSTR pzFile)
		{
			if (!pzFile)
				return FALSE;
			if (m_hmmio != NULL)
			{
				mmioClose(m_hmmio, 0);
				m_hmmio = NULL;
			}
			if ((m_hmmio = mmioOpen(pzFile, NULL, MMIO_READWRITE | MMIO_ALLOCBUF)) == NULL)
				return FALSE;
			m_reading = TRUE;
			MMCKINFO    mmckRIFF = { 0 };
			MMCKINFO    mmck = { 0 };
			MMRESULT    mmRes = MMSYSERR_NOERROR;
			WORD		wFormatSize = 0;
			mmckRIFF.fccType = mmioWAVE;
			mmRes = mmioDescend(m_hmmio, &mmckRIFF, NULL, MMIO_FINDRIFF);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			mmck.ckid = mmioFMT;
			mmRes = mmioDescend(m_hmmio, &mmck, &mmckRIFF, MMIO_FINDCHUNK);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			if (mmck.cksize < sizeof(WAVEFORMAT))
				goto MMIO_ERROR;
			wFormatSize = (WORD)mmck.cksize;
			m_waveEx.Reset(new BYTE[wFormatSize]);
			if ((DWORD)mmioRead(m_hmmio, (HPSTR)m_waveEx.Ptr(), mmck.cksize) != mmck.cksize)//读取WAVEFORMAT
				goto MMIO_ERROR;
			WAVEFORMATEX* waveEx = reinterpret_cast<WAVEFORMATEX*>(m_waveEx.Ptr());
			if (waveEx->wFormatTag == WAVE_FORMAT_PCM)//PCM格式
			{
				if (wFormatSize < sizeof(PCMWAVEFORMAT))
					goto MMIO_ERROR;
				waveEx->cbSize = 0;
			}
			else
			{
				if ((wFormatSize < sizeof(WAVEFORMATEX)) ||
					(wFormatSize < sizeof(WAVEFORMATEX) + waveEx->cbSize))
					goto MMIO_ERROR;
			}
			mmRes = mmioAscend(m_hmmio, &mmck, 0);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			mmck.ckid = mmioDATA;
			mmRes = mmioDescend(m_hmmio, &mmck, &mmckRIFF, MMIO_FINDCHUNK);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			m_dwLSize = m_dwSize = mmck.cksize;
			m_dwDataOffset = mmck.dwDataOffset;
			if (-1 == mmioSeek(m_hmmio, mmck.dwDataOffset, SEEK_SET))
				goto MMIO_ERROR;
			return TRUE;
		MMIO_ERROR:
			mmioClose(m_hmmio, 0);
			m_hmmio = NULL;
			return FALSE;
		}
		BOOL TinyWaveFile::Open(LPVOID pStream, LONG bufferSize)
		{
			if (pStream == NULL)
				return FALSE;
			if (m_hmmio != NULL)
			{
				mmioClose(m_hmmio, 0);
				m_hmmio = NULL;
			}
			WAVEFORMATEX waveEx = { 0 };
			//内存流
			MMIOINFO mmioInfo;
			ZeroMemory(&mmioInfo, sizeof(MMIOINFO));
			mmioInfo.fccIOProc = FOURCC_MEM;
			mmioInfo.cchBuffer = bufferSize;
			mmioInfo.pchBuffer = (CHAR*)pStream;
			if ((m_hmmio = mmioOpen(NULL, &mmioInfo, MMIO_READ | MMIO_ALLOCBUF)) == NULL)
				return FALSE;
			m_reading = TRUE;
			MMCKINFO    mmckRIFF = { 0 };
			MMCKINFO    mmck = { 0 };
			MMRESULT    mmRes = MMSYSERR_NOERROR;
			WORD		wFormatSize = 0;
			mmckRIFF.fccType = mmioWAVE;
			mmRes = mmioDescend(m_hmmio, &mmckRIFF, NULL, MMIO_FINDRIFF);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			mmck.ckid = mmioFMT;
			mmRes = mmioDescend(m_hmmio, &mmck, &mmckRIFF, MMIO_FINDCHUNK);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			if (mmck.cksize < sizeof(WAVEFORMAT))
				goto MMIO_ERROR;
			wFormatSize = (WORD)mmck.cksize;
			if ((DWORD)mmioRead(m_hmmio, (HPSTR)&m_waveEx, mmck.cksize) != mmck.cksize)//读取WAVEFORMAT
				goto MMIO_ERROR;
			if (waveEx.wFormatTag == WAVE_FORMAT_PCM)//PCM格式
			{
				if (wFormatSize < sizeof(PCMWAVEFORMAT))
					goto MMIO_ERROR;
				m_waveEx.Reset(new BYTE[sizeof(WAVEFORMATEX)]);
				memcpy(m_waveEx, &waveEx, sizeof(WAVEFORMATEX));
			}
			else
			{
				if ((wFormatSize < sizeof(WAVEFORMATEX)) || (wFormatSize < sizeof(WAVEFORMATEX) + waveEx.cbSize))
					goto MMIO_ERROR;
				if (waveEx.cbSize != 0)
				{
					LPBYTE extraBytes = (LPBYTE)LocalAlloc(LPTR, waveEx.cbSize);
					if (extraBytes != NULL)
					{
						if (mmioRead(m_hmmio, (HPSTR)extraBytes, waveEx.cbSize) != waveEx.cbSize)
						{
							m_waveEx.Reset(new BYTE[sizeof(WAVEFORMATEX) + waveEx.cbSize]);
							memcpy(m_waveEx, &waveEx, sizeof(WAVEFORMATEX));
							memcpy(m_waveEx + sizeof(WAVEFORMATEX), (BYTE*)&waveEx + sizeof(WAVEFORMATEX), waveEx.cbSize);
							SAFE_LOCAL_FREE(extraBytes);
							goto MMIO_ERROR;
						}
					}
				}
			}
			mmRes = mmioAscend(m_hmmio, &mmck, 0);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			mmck.ckid = mmioDATA;
			mmRes = mmioDescend(m_hmmio, &mmck, &mmckRIFF, MMIO_FINDCHUNK);
			if (mmRes != MMSYSERR_NOERROR)
				goto MMIO_ERROR;
			m_dwLSize = m_dwSize = mmck.cksize;
			m_dwDataOffset = mmck.dwDataOffset;
			if (-1 == mmioSeek(m_hmmio, mmck.dwDataOffset, SEEK_SET))
				goto MMIO_ERROR;
			return TRUE;
		MMIO_ERROR:
			mmioClose(m_hmmio, 0);
			m_hmmio = NULL;
			return FALSE;
		}
		void TinyWaveFile::Close()
		{
			if (m_hmmio != NULL)
			{
				if (!m_reading)
				{
					mmioAscend(m_hmmio, &m_mmckDATA, 0);
					mmioAscend(m_hmmio, &m_mmckRIFF, 0);
				}
				mmioClose(m_hmmio, 0);
				m_hmmio = NULL;
			}
			m_dwSize = m_dwLSize = m_dwDataOffset = 0;
		}
		BOOL TinyWaveFile::Read(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LPLONG lpNumberOfBytesRead)
		{
			if (!m_hmmio)
				return FALSE;
			*lpNumberOfBytesRead = 0;
			MMIOINFO mmioinfo;
			MMRESULT mmRes = MMSYSERR_NOERROR;
			mmRes = mmioGetInfo(m_hmmio, &mmioinfo, 0);
			if (mmRes != MMSYSERR_NOERROR)
				return FALSE;
			UINT cbDataIn = nNumberOfBytesToRead;
			if (cbDataIn > m_dwSize) cbDataIn = m_dwLSize;
			m_dwLSize -= cbDataIn;
			for (DWORD cT = 0; cT < cbDataIn; cT++)
			{
				if (mmioinfo.pchNext == mmioinfo.pchEndRead)
				{
					mmRes = mmioAdvance(m_hmmio, &mmioinfo, MMIO_READ);
					if (mmRes != MMSYSERR_NOERROR)
						return FALSE;
					if (mmioinfo.pchNext == mmioinfo.pchEndRead)
						return FALSE;
				}
				*((BYTE*)lpBuffer + cT) = *((BYTE*)mmioinfo.pchNext);
				mmioinfo.pchNext++;
			}
			mmRes = mmioSetInfo(m_hmmio, &mmioinfo, 0);
			if (mmRes != MMSYSERR_NOERROR)
				return FALSE;
			*lpNumberOfBytesRead = cbDataIn;
			return TRUE;
		}
		BOOL TinyWaveFile::Seek(LONG lOffset, INT iOrigin)
		{
			if (!m_hmmio)
				return FALSE;
			if (-1 == mmioSeek(m_hmmio, lOffset, iOrigin))
				return FALSE;
			return TRUE;
		}
		WAVEFORMATEX* TinyWaveFile::GetFormat() const
		{
			return reinterpret_cast<WAVEFORMATEX*>(m_waveEx.Ptr());
		}
		DWORD TinyWaveFile::GetSize() const
		{
			return m_dwSize;
		}
		DWORD TinyWaveFile::GetDataOffset() const
		{
			return m_dwDataOffset;
		}
		BOOL TinyWaveFile::ResetFile()
		{
			return Seek(m_dwDataOffset, SEEK_SET);
		}
	};
}
