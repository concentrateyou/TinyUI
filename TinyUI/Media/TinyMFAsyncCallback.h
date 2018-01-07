#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// MF“Ï≤Ω
		/// </summary>
		class TinyMFAsyncCallback : public TinyReference<TinyMFAsyncCallback>, public IMFAsyncCallback
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFAsyncCallback)
		public:
			TinyMFAsyncCallback();
			virtual ~TinyMFAsyncCallback();
			BOOL Initialize(IMFTransform* pMFT);
		public:
			HRESULT STDMETHODCALLTYPE GetParameters(__RPC__out DWORD *pdwFlags, __RPC__out DWORD *pdwQueue) OVERRIDE;
			HRESULT STDMETHODCALLTYPE Invoke(__RPC__in_opt IMFAsyncResult *pAsyncResult) OVERRIDE;
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
			ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
			ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
			virtual void OnInvokeInput() = 0;
			virtual void OnInvokeOutput() = 0;
		protected:
			LONG								m_inputs;
			LONG								m_outputs;
			IMFTransform*						m_pMFT;
			TinyComPtr<IMFMediaEventGenerator>	m_eventGenerator;
		};
	};
}
