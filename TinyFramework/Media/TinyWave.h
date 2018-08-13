#pragma once
#include "../Common/TinyEvent.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace TinyFramework
{
	namespace Media
	{
#define mmioRIFF    mmioFOURCC('R', 'I', 'F', 'F');
#define mmioWAVE    mmioFOURCC('W','A','V','E')
#define mmioFMT     mmioFOURCC('f','m','t',' ')
#define mmioDATA    mmioFOURCC('d','a','t','a')
#define mmioFACT    mmioFOURCC('f','a','c','t')
#define WAVE_READ   0;
#define WAVE_WRITE  1;
#define PI 3.14159265
#define PLAY_QUIT (WM_USER + 0x0123)//线程间发送播放退出消息

#define LoopEmpty() //空的循环

#define MAX_AUDIO_BUFFERS	8
#define MIN_AUDIO_BUFFERS	2
#define AUDIO_BUFFER_SIZE	4096
#define DSOUND_BUFFERS		3

		/// <summary>
		/// Wave文件操作类
		/// </summary>
		class TinyWaveFile
		{
		public:
			TinyWaveFile();
			~TinyWaveFile();
			operator HMMIO() const throw();;
			BOOL Create(LPTSTR pzFile, const WAVEFORMATEX* pWaveEx);
			BOOL Open(LPTSTR pzFile);
			BOOL Open(LPVOID pStream, LONG bufferSize);
			BOOL Read(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LPLONG lpNumberOfBytesRead);
			LONG Write(BYTE* lpBuffer, LONG nNumberOfBytesToRead);
			BOOL Seek(LONG lOffset, INT iOrigin);
			BOOL ResetFile();
			void Close();
			WAVEFORMATEX* GetFormat() const;
			WORD  GetFormatSize() const;
			DWORD GetSize() const;
			DWORD GetDataOffset() const;
		private:
			BOOL					m_reading;
			DWORD					m_dwSize;//数据大小
			DWORD					m_dwLSize;//剩余数据大小
			DWORD					m_dwDataOffset;//波形数据的偏移量
			HMMIO					m_hmmio;
			MMCKINFO				m_mmckRIFF;
			MMCKINFO				m_mmckFMT;
			MMCKINFO				m_mmckFACT;
			MMCKINFO				m_mmckDATA;
			WORD					m_wFormatSize;
			TinyScopedArray<BYTE>	m_waveFMT;
		};
		/// <summary>
		/// Wave输入类
		/// </summary>
		class TinyWaveIn
		{
		private:
			HWAVEIN	m_hWAVE;
		public:
			TinyWaveIn();
			~TinyWaveIn();
			operator HWAVEIN() const throw();
			BOOL QueryFormat(LPWAVEFORMATEX pwfx);
			BOOL Open(LPWAVEFORMATEX pwfx, DWORD_PTR dwCallbackInstance);
			BOOL Start();
			BOOL Stop();
			BOOL Reset();
			BOOL Close();
			BOOL Prepare(LPWAVEHDR pwh);
			BOOL Unprepare(LPWAVEHDR pwh);
			BOOL Add(LPWAVEHDR pwh);
			BOOL GetPosition(LPMMTIME pmmt);
			void	GetErrorText(LPTSTR pzText, MMRESULT hRes);
			virtual void OnOpen();
			virtual void OnClose();
			virtual void OnData(LPWAVEHDR wp, DWORD_PTR dwInstance);
		private:
			static void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
		};
		/// <summary>
		/// Wave输出类
		/// </summary>
		class TinyWaveOut
		{
		private:
			HWAVEOUT m_hWAVE;
		public:
			TinyWaveOut();
			~TinyWaveOut();
			operator HWAVEOUT() const throw();
			BOOL QueryFormat(LPWAVEFORMATEX pwfx);
			BOOL Open(LPWAVEFORMATEX pwfx, DWORD_PTR dwCallbackInstance);
			BOOL Pause();
			BOOL Reset();
			BOOL Restart();
			BOOL Close();
			BOOL SetVolume(DWORD dwVolume);
			BOOL GetDevCaps(UINT_PTR uDeviceID, LPWAVEOUTCAPS pwoc);
			BOOL GetPosition(LPMMTIME pmmt);
			BOOL Prepare(LPWAVEHDR pwh);
			BOOL Unprepare(LPWAVEHDR pwh);
			BOOL BreakLoop();
			BOOL Write(LPWAVEHDR pwh);
			BOOL SetPlaybackRate(DWORD dwRate);
			void	SendMessage(UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
			DWORD	GetVolume();
			DWORD	GetNumDevs();
			void	GetErrorText(LPTSTR pzText, MMRESULT hRes);
			virtual void OnOpen();
			virtual void OnClose();
			virtual void OnDone(LPWAVEHDR wp, DWORD_PTR dwInstance);
			Event<void(LPWAVEHDR, DWORD_PTR)> WaveDone;
		private:
			static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
		};
		/// <summary>
		/// Wave播放类
		/// </summary>
		class TinyWavePlayer
		{
		public:
			TinyWavePlayer();
			~TinyWavePlayer();
			BOOL LoadFile(LPSTR szFilename);
			BOOL LoadStream(LPVOID pStream, DWORD bufferSize);
			BOOL IsPlaying();
			BOOL Play(BOOL bLoop);
			BOOL Seek(DWORD newPos);
			void PlayDone(LPWAVEHDR wp, DWORD_PTR dwInstance);
			BOOL Pause();
			BOOL Restart();
			BOOL Reset();
			DWORD GetDataSize();
		private:
			TinyWaveFile	m_waveFile;
			TinyWaveOut		m_waveOut;
			LPWAVEHDR		m_audioBlockPtr[MAX_AUDIO_BUFFERS];
			DWORD			m_dwBlocks;
			DWORD			m_dwBlockOut;
			DWORD			m_dwBlockNext;
			DWORD			m_dwSizeL;//播放数据
			BOOL			m_bPlaying;
			void PlayData();
		};
		/// <summary>
		/// Wave录音机
		/// </summary>
		class TinyWaveTape
		{
		public:
			TinyWaveTape();
			~TinyWaveTape();
			BOOL LoadFile(LPTSTR pzFile);
			BOOL Start();
			BOOL Stop();
			BOOL Reset();
		private:
			TinyWaveFile	m_waveFile;
			TinyWaveIn		m_waveOut;
			LPWAVEHDR		m_audioBlockPtr[MAX_AUDIO_BUFFERS];
			DWORD			m_dwBlocks;
			TinyScopedPtr<Delegate<void(LPWAVEHDR, DWORD_PTR)>> m_waveTapeDone;
		};
	};
}


