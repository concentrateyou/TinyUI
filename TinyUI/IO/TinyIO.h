#pragma once
#include "../Common/TinyCommon.h"
#include "../Common/TinyTime.h"

namespace TinyUI
{
	namespace IO
	{
#define SIZEINCR    0x1000
		struct FileStatus
		{
			TinyTime m_ctime;
			TinyTime m_mtime;
			TinyTime m_atime;
			ULONGLONG m_size;
			BYTE m_attribute;
			BYTE _m_padding;
			TCHAR m_szFullName[_MAX_PATH];
		};
		//虚基类
		class TinyIO : public TinyObject
		{
			DECLARE_DYNAMIC(TinyIO)
		public:
			virtual LONGLONG Seek(LONGLONG lOff, DWORD dwMoveMethod) = 0;
			virtual DWORD Read(void* pData, DWORD cbData) = 0;
			virtual DWORD Write(const void* pData, DWORD cbData) = 0;
			virtual BOOL LockRange(ULONGLONG dwPos, ULONGLONG dwCount) = 0;
			virtual BOOL UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount) = 0;
			virtual BOOL Flush() = 0;
			virtual BOOL Close() = 0;
			virtual LONGLONG GetPosition() const = 0;
			virtual LONGLONG GetSize() const = 0;
		};
		/// <summary>
		/// 文件操作类
		/// </summary>
		class TinyFile : public TinyIO
		{
			DECLARE_DYNAMIC(TinyFile)
		public:
			static const HANDLE hFileNull;
			TinyFile();
			TinyFile(HANDLE hFile);
			virtual ~TinyFile();
			HANDLE Handle() const;
			BOOL GetStatus(FileStatus& rStatus) const;
			virtual BOOL Create(LPCTSTR lpszFileName, DWORD dwFlagsAndAttributes = 0);
			virtual BOOL Open(LPCTSTR lpszFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwFlagsAndAttributes);
			virtual LPTSTR GetTitle() const;
			virtual LPTSTR GetPath() const;
			virtual BOOL SetSize(ULARGE_INTEGER dwNewLen);
			virtual DWORD GetType() const;
			//////////////////////////////////////////////////////////////////////////
			ULONGLONG SeekToEnd();
			ULONGLONG SeekToBegin();
			LONGLONG Seek(LONGLONG lOff, DWORD dwMoveMethod) OVERRIDE;
			DWORD Read(void* pData, DWORD cbData) OVERRIDE;
			DWORD Write(const void* pData, DWORD cbData) OVERRIDE;
			BOOL LockRange(ULONGLONG dwPos, ULONGLONG dwCount) OVERRIDE;
			BOOL UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount) OVERRIDE;
			BOOL Flush() OVERRIDE;
			BOOL Close() OVERRIDE;
			LONGLONG GetPosition() const OVERRIDE;
			LONGLONG GetSize() const OVERRIDE;
			//////////////////////////////////////////////////////////////////////////
			static BOOL PASCAL GetStatus(LPCTSTR lpszFileName, FileStatus& rStatus);
			static void PASCAL SetStatus(LPCTSTR lpszFileName, const FileStatus& status);
			static BOOL Exists(LPCSTR pszPath);
			static BOOL Rename(LPCSTR lpszOldName, LPCTSTR lpszNewName);
			static BOOL Remove(LPCSTR lpszFileName);
			static BOOL GetFileNameFromHandle(HANDLE hFile, LPTSTR pszFileName);//获得文件的绝对路径
		protected:
			HANDLE	m_hFile;
			LPCTSTR m_pzFileName;
			TCHAR	m_pzFileTitle[MAX_PATH];
			TCHAR	m_pzPath[MAX_PATH];
		};
		/// <summary>
		/// 标准库IO
		/// </summary>
		class TinyStdioFile : public TinyIO
		{
			DECLARE_DYNAMIC(TinyStdioFile)
		public:
			TinyStdioFile(FILE* pOpenStream);
			TinyStdioFile(LPCTSTR lpszFileName, CHAR mode);
			virtual ~TinyStdioFile();
		public:
			LONGLONG Seek(LONGLONG lOff, DWORD dwMoveMethod) OVERRIDE;
			DWORD Read(void* pData, DWORD cbData) OVERRIDE;
			DWORD Write(const void* pData, DWORD cbData) OVERRIDE;
			BOOL LockRange(ULONGLONG dwPos, ULONGLONG dwCount) OVERRIDE;
			BOOL UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount) OVERRIDE;
			BOOL Flush() OVERRIDE;
			BOOL Close() OVERRIDE;
			LONGLONG GetPosition() const OVERRIDE;
			LONGLONG GetSize() const OVERRIDE;
		protected:
			FILE*	m_hFile;
		};
		/// <summary>
		/// 内存文件
		/// </summary>
		class TinyMemoryFile : public TinyIO
		{
			DECLARE_DYNAMIC(TinyMemoryFile)
		public:
			virtual LONGLONG Seek(LONGLONG lOff, DWORD dwMoveMethod);
			virtual DWORD Read(void* pData, DWORD cbData);
			virtual DWORD Write(const void* pData, DWORD cbData);
			virtual BOOL LockRange(ULONGLONG dwPos, ULONGLONG dwCount);
			virtual BOOL UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount);
			virtual BOOL Flush();
			virtual BOOL Close();
		public:
			TinyMemoryFile(BYTE* pBuffer = NULL, LONGLONG dwBufferSize = 0);
			virtual ~TinyMemoryFile();
			virtual LONGLONG GetPosition() const;
			virtual BOOL SetSize(LONGLONG dwNewLen);
			virtual LONGLONG GetSize() const;
		private:
			BOOL Realloc(LONGLONG dwNewLen);
		private:
			BYTE*		m_pBuffer;
			LONGLONG	m_dwBufferSize;
			LONGLONG	m_dwPosition;
		};
		/// <summary>
		/// 文件流
		/// </summary>
		class TinyFileStream : public IStream
		{
		public:
			// *** IUnknown methods ***
			STDMETHOD(QueryInterface) (REFIID riid, void **ppvObj);
			STDMETHOD_(ULONG, AddRef) ();
			STDMETHOD_(ULONG, Release) ();
			// *** IStream methods ***
			STDMETHOD(Read) (VOID *pv, ULONG cb, ULONG *pcbRead);
			STDMETHOD(Write) (VOID const *pv, ULONG cb, ULONG *pcbWritten);
			STDMETHOD(Seek) (LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
			STDMETHOD(SetSize) (ULARGE_INTEGER libNewSize);
			STDMETHOD(CopyTo) (IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
			STDMETHOD(Commit) (DWORD grfCommitFlags);
			STDMETHOD(Revert) ();
			STDMETHOD(LockRegion) (ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
			STDMETHOD(UnlockRegion) (ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
			STDMETHOD(Stat) (STATSTG *pstatstg, DWORD grfStatFlag);
			STDMETHOD(Clone)(IStream **ppstm);
		public:
			TinyFileStream(HANDLE hf, DWORD grfMode);
			virtual~TinyFileStream();
		private:
			LONG        m_cRef;
			HANDLE      m_hFile;
		};
		/// <summary>
		/// 内存流
		/// </summary>
		class TinyMemoryStream : public IStream
		{
		public:
			// *** IUnknown methods ***
			STDMETHOD(QueryInterface)(REFIID riid, void **ppvObj);
			STDMETHOD_(ULONG, AddRef)();
			STDMETHOD_(ULONG, Release)();
			// *** IStream methods ***
			STDMETHOD(Read)(void *pv, ULONG cb, ULONG *pcbRead);
			STDMETHOD(Write)(void const *pv, ULONG cb, ULONG *pcbWritten);
			STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
			STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize);
			STDMETHOD(CopyTo)(IStream *, ULARGE_INTEGER, ULARGE_INTEGER *, ULARGE_INTEGER *);
			STDMETHOD(Commit)(DWORD);
			STDMETHOD(Revert)();
			STDMETHOD(LockRegion)(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
			STDMETHOD(UnlockRegion)(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
			STDMETHOD(Stat)(STATSTG *, DWORD);
			STDMETHOD(Clone)(IStream **);
			LPBYTE ReAlloc(ULONG cbNew);
		public:
			TinyMemoryStream();
			virtual~TinyMemoryStream();
		public:
			LONG        m_cRef;
			LPBYTE      m_pData;
			UINT        m_cbAlloc;
			UINT        m_cbData;
			UINT        m_iSeek;
		};
	};
}


