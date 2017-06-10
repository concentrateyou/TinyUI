#pragma once
#include <string>
#include "IO/TinyIO.h"
#include "IO/TinyRingBuffer.h"
#include "rtmp.h"
using namespace std;

namespace Decode
{
	/// <summary>
	/// RTMPÕ¯¬Á¡˜ ‘”√Õ¯¬Á—”≥ŸµÕ
	/// </summary>
	class RTMPStream : public IStream
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
		RTMPStream();
		virtual~RTMPStream();
		BOOL Open(LPCSTR pzURL);
		BOOL Close();
	private:
		LONG			m_cRef;
		RTMP			m_sRTMP;
	};
}

