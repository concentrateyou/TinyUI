#include "stdafx.h"
#include "QSVCommon.h"
#include <ppl.h>

namespace QSV
{
	D3DFORMAT ConvertMfxFourccToD3dFormat(mfxU32 fourcc)
	{
		switch (fourcc)
		{
		case MFX_FOURCC_NV12:
			return D3DFMT_NV12;
		case MFX_FOURCC_YV12:
			return D3DFMT_YV12;
		case MFX_FOURCC_NV16:
			return D3DFMT_NV16;
		case MFX_FOURCC_YUY2:
			return D3DFMT_YUY2;
		case MFX_FOURCC_RGB3:
			return D3DFMT_R8G8B8;
		case MFX_FOURCC_RGB4:
			return D3DFMT_A8R8G8B8;
		case MFX_FOURCC_P8:
			return D3DFMT_P8;
		case MFX_FOURCC_P010:
			return D3DFMT_P010;
		case MFX_FOURCC_P210:
			return D3DFMT_P210;
		case MFX_FOURCC_A2RGB10:
			return D3DFMT_A2R10G10B10;
		case MFX_FOURCC_ABGR16:
		case MFX_FOURCC_ARGB16:
			return D3DFMT_A16B16G16R16;
		case MFX_FOURCC_IMC3:
			return D3DFMT_IMC3;
		default:
			return D3DFMT_UNKNOWN;
		}
	}

	BOOL IsSSE41Enabled()
	{
		int CPUInfo[4];
		__cpuid(CPUInfo, 1);

		return 0 != (CPUInfo[2] & (1 << 19));
	}
	BOOL IsAVX2Enabled()
	{
		int CPUInfo[4];
		__cpuid(CPUInfo, 7);

		return 0 != (CPUInfo[1] & (1 << 5));
	}

	static BOOL g_SSE4_1_enabled = IsSSE41Enabled();
	static BOOL g_AVX2_enabled = IsAVX2Enabled();

#define MIN_BUFF_SIZE (1 << 18)
	typedef void* (*my_memcpy)(void*, const void*, size_t);
	static void* mt_copy(void* d, const void* s, size_t size, my_memcpy mymemcpy)
	{
		MSDK_CHECK_POINTER(d, NULL);
		MSDK_CHECK_POINTER(s, NULL);
		if (size < MIN_BUFF_SIZE)
		{
			return mymemcpy(d, s, size);
		}
		size_t blockSize = (size / 2) & ~31;
		const size_t offsets[] = { 0, blockSize, size };
		Concurrency::parallel_for(0, 2, [d, s, &offsets, mymemcpy](int i)
		{
			mymemcpy((char*)d + offsets[i], (const char*)s + offsets[i], offsets[i + 1] - offsets[i]);
		});
		return d;
	}

	void* gpu_memcpy_sse41(void* d, const void* s, size_t size)
	{
		static const size_t regsInLoop = 2;
		if (d == NULL || s == NULL)
			return NULL;
		bool isAligned = (((size_t)(s) | (size_t)(d)) & 0xF) == 0;
		if (!(isAligned && g_SSE4_1_enabled))
		{
			return memcpy(d, s, size);
		}
		size_t reminder = size & (regsInLoop * sizeof(__m128i) - 1);
		size_t end = 0;

		__m128i xmm0, xmm1;
		__m128i* pTrg = (__m128i*)d;
		__m128i* pTrgEnd = pTrg + ((size - reminder) >> 4);
		__m128i* pSrc = (__m128i*)s;
		_mm_sfence();

		while (pTrg < pTrgEnd)
		{
			xmm0 = _mm_stream_load_si128(pSrc);
			xmm1 = _mm_stream_load_si128(pSrc + 1);
			pSrc += regsInLoop;
			_mm_store_si128(pTrg, xmm0);
			_mm_store_si128(pTrg + 1, xmm1);
			_mm_store_si128(pTrg + 1, xmm1);
			_mm_store_si128(pTrg + 1, xmm1);
			pTrg += regsInLoop;
		}
		if (reminder >= 16)
		{
			size = reminder;
			reminder = size & 15;
			end = size >> 4;
			for (size_t i = 0; i < end; ++i)
			{
				pTrg[i] = _mm_stream_load_si128(pSrc + i);
			}
		}
		if (reminder)
		{
			__m128i temp = _mm_stream_load_si128(pSrc + end);
			char* ps = (char*)(&temp);
			char* pt = (char*)(pTrg + end);
			for (size_t i = 0; i < reminder; ++i)
			{
				pt[i] = ps[i];
			}
		}
		return d;
	}
	void* gpu_memcpy_avx2(void* d, const void* s, size_t size)
	{
		static const size_t regsInLoop = 4;
		if (d == NULL || s == NULL) return NULL;
		bool isAligned = (((size_t)(s) | (size_t)(d)) & 0x1F) == 0;
		if (!(isAligned && g_SSE4_1_enabled))
		{
			return gpu_memcpy_sse41(d, s, size);
		}
		size_t reminder = size & (regsInLoop * sizeof(__m256i) - 1);
		size_t end = 0;
		__m256i ymm0, ymm1, ymm2, ymm3;
		__m256i* pTrg = (__m256i*)d;
		__m256i* pTrgEnd = pTrg + ((size - reminder) >> 5);
		__m256i* pSrc = (__m256i*)s;
		_mm_sfence();

		while (pTrg < pTrgEnd)
		{
			ymm0 = _mm256_stream_load_si256(pSrc);
			ymm1 = _mm256_stream_load_si256(pSrc + 1);
			ymm2 = _mm256_stream_load_si256(pSrc + 2);
			ymm3 = _mm256_stream_load_si256(pSrc + 3);
			pSrc += regsInLoop;
			_mm256_store_si256(pTrg, ymm0);
			_mm256_store_si256(pTrg + 1, ymm1);
			_mm256_store_si256(pTrg + 2, ymm2);
			_mm256_store_si256(pTrg + 3, ymm3);
			pTrg += regsInLoop;
		}
		if (reminder >= 32)
		{
			size = reminder;
			reminder = size & 31;
			end = size >> 5;
			for (size_t i = 0; i < end; ++i)
			{
				pTrg[i] = _mm256_stream_load_si256(pSrc + i);
			}
		}
		if (reminder)
		{
			__m256i temp = _mm256_stream_load_si256(pSrc + end);
			char* ps = (char*)(&temp);
			char* pt = (char*)(pTrg + end);
			for (size_t i = 0; i < reminder; ++i)
			{
				pt[i] = ps[i];
			}
		}
		return d;
	}

	void* mt_memcpy(void* d, const void* s, size_t size)
	{
		return mt_copy(d, s, size, memcpy);
	}
	void* mt_gpu_memcpy(void* d, const void* s, size_t size)
	{
		return (g_AVX2_enabled) ?
			mt_copy(d, s, size, gpu_memcpy_avx2) :
			mt_copy(d, s, size, gpu_memcpy_sse41);
	}

	mfxU32 GetIntelAdapter(mfxSession session)
	{
		mfxU32  adapters = 0;
		mfxIMPL impl = MFX_IMPL_SOFTWARE;
		if (session)
		{
			MFXQueryIMPL(session, &impl);
		}
		else
		{
			mfxSession auxSession;
			memset(&auxSession, 0, sizeof(auxSession));
			mfxVersion ver = { 1, 1 };
			MFXInit(MFX_IMPL_HARDWARE_ANY, &ver, &auxSession);
			MFXQueryIMPL(auxSession, &impl);
			MFXClose(auxSession);
		}
		mfxIMPL baseImpl = MFX_IMPL_BASETYPE(impl);
		for (mfxU8 i = 0; i < sizeof(implTypes) / sizeof(implTypes[0]); i++)
		{
			if (implTypes[i].impl == baseImpl)
			{
				adapters = implTypes[i].adapterID;
				break;
			}
		}
		return adapters;
	}
	INT GetMSDKAdapters(mfxSession session)
	{
		INT adapters = -1;
		mfxIMPL impl = MFX_IMPL_SOFTWARE;
		if (session)
		{
			MFXQueryIMPL(session, &impl);
		}
		else
		{
			mfxSession auxSession;
			memset(&auxSession, 0, sizeof(auxSession));

			mfxVersion ver = { 1, 1 };
			MFXInit(MFX_IMPL_HARDWARE_ANY, &ver, &auxSession);
			MFXQueryIMPL(auxSession, &impl);
			MFXClose(auxSession);
		}
		mfxIMPL baseImpl = MFX_IMPL_BASETYPE(impl);
		for (mfxU8 i = 0; i < sizeof(implTypes) / sizeof(implTypes[0]); ++i)
		{
			if (implTypes[i].impl == baseImpl)
			{
				adapters = implTypes[i].adapterID;
				break;
			}
		}
		return adapters;
	}
	mfxU16 GetFreeSurfaceIndex(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize)
	{
		if (pSurfacesPool)
		{
			for (mfxU16 i = 0; i < nPoolSize; i++)
			{
				if (0 == pSurfacesPool[i].Data.Locked)
				{
					return i;
				}
			}
		}
		return MSDK_INVALID_SURF_IDX;
	}
}