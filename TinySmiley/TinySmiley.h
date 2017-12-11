#pragma once
//http://www.codeproject.com/Articles/8679/Building-a-LOCAL-COM-Server-and-Client-A-Step-by-S
//http://www.codeproject.com/Articles/338268/COM-in-C
//http://www.cppblog.com/mzty/archive/2007/05/28/25007.html
#include "smiley_h.h"
#include <windows.h>
#include <vector>
#include <atlbase.h>
#include <Richedit.h>
#include <RichOle.h>
using namespace std;

static const GUID  CLSID_Smiley =
{ 0xB6AD5231, 0xC3EA, 0x4261, { 0xA9, 0xB6, 0xC5, 0x7C, 0xC7, 0x20, 0xB4, 0xC6 } };

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if (p) { delete (p);  (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY 
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p)=NULL; } }
#endif  
#ifndef SAFE_FREE
#define SAFE_FREE(p) { if (p) { free (p); (p)=NULL; } }
#endif   
#ifndef SAFE_DELETE_OBJECT
#define SAFE_DELETE_OBJECT(p) { if (p) { DeleteObject(p); (p)=NULL; } }
#endif    
#ifndef TO_CX
#define TO_CX(rect)    abs(rect.right-rect.left)
#endif

#ifndef TO_CY
#define TO_CY(rect)    abs(rect.bottom-rect.top)
#endif

#ifndef TINY_EXPORT
#define TINY_EXPORT __declspec(dllexport)
#endif // TINY_EXPORT

#ifndef TINY_IMPORT
#define TINY_IMPORT __declspec(dllimport)
#endif // TINY_IMPORT

namespace Smiley
{
	/// <summary>
	/// ������
	/// </summary>
	class TinySmiley : public ISmiley
	{
	public:
		TinySmiley();
		~TinySmiley();
		virtual HRESULT STDMETHODCALLTYPE LoadStream(LPSTREAM pStm);
		virtual HRESULT STDMETHODCALLTYPE LoadFile(LPCSTR pszFilePath);
		virtual HRESULT STDMETHODCALLTYPE SaveAs(LPSTREAM pStm);
		virtual HRESULT STDMETHODCALLTYPE GetFrameCount(INT *pFrameCount);
		virtual HRESULT STDMETHODCALLTYPE GetFrameDelay(INT iFrame, UINT *pFrameDelay);
		virtual HRESULT STDMETHODCALLTYPE GetSize(LPSIZE pSize);
		virtual HRESULT STDMETHODCALLTYPE Draw(HDC hdc, LPCRECT pRect, INT iFrame);
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef(void);
		virtual ULONG STDMETHODCALLTYPE Release(void);
	private:
		vector<HBITMAP> m_images;
		vector<UINT>	m_delays;
		LONG    m_cRef;
		INT		m_cx;
		INT		m_cy;
		INT		m_count;//֡��
	};
	//////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// ����OLE
	/// </summary>
	/*class ATL_NO_VTABLE TinySmileyCtrl : public CComControl<TinySmileyCtrl>,
		public CStockPropImpl<TinySmileyCtrl, ISmileyCtrl>,
		public IOleObjectImpl<TinySmileyCtrl>,
		public IOleInPlaceObjectWindowlessImpl<TinySmileyCtrl>,
		public IViewObjectExImpl<TinySmileyCtrl>,
		public IPersistStreamInitImpl<TinySmileyCtrl>,
		public IPersistStorageImpl<TinySmileyCtrl>,
		public IOleControlImpl < TinySmileyCtrl >
	{
		DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
		OLEMISC_CANTLINKINSIDE |
			OLEMISC_INSIDEOUT |
			OLEMISC_ACTIVATEWHENVISIBLE |
			OLEMISC_SETCLIENTSITEFIRST)

			BEGIN_COM_MAP(TinySmileyCtrl)
				COM_INTERFACE_ENTRY(ISmileyCtrl)
				COM_INTERFACE_ENTRY(IOleControl)
				COM_INTERFACE_ENTRY(IOleObject)
				COM_INTERFACE_ENTRY(IPersistStreamInit)
				COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
				COM_INTERFACE_ENTRY(IPersistStorage)
			END_COM_MAP()

			BEGIN_PROP_MAP(TinySmileyCtrl)
			END_PROP_MAP()

			BEGIN_MSG_MAP(TinySmileyCtrl)
				CHAIN_MSG_MAP(CComControl<TinySmileyCtrl>)
				DEFAULT_REFLECTION_HANDLER()
			END_MSG_MAP()

			DECLARE_PROTECT_FINAL_CONSTRUCT()
	public:
		TinySmileyCtrl();
		~TinySmileyCtrl();
	public:
		STDMETHOD(InsertOLE)(DWORD_PTR ole);
		STDMETHOD(GetSmiley)(ISmiley* ps);
		STDMETHOD(SetSmiley)(ISmiley** ps);
	public:
		STDMETHOD(SetClientSite)(IOleClientSite *pClientSite);
	public:
		HRESULT IPersistStreamInit_Load(LPSTREAM pStm, const ATL_PROPMAP_ENTRY* pMap);
		HRESULT IPersistStreamInit_Save(LPSTREAM pStm, BOOL fClearDirty, const ATL_PROPMAP_ENTRY* pMap);
		HRESULT FireViewChange();
		HRESULT	OnDraw(ATL_DRAWINFO& di);
	public:
		STDMETHOD(Load)(IStorage* pStorage);
	private:
		CComPtr<ISmiley>  m_pSmiley;
	};*/
	//////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// COM������
	/// </summary>
	template<class T>
	class ClassFactory : public IClassFactory
	{
	public:
		ClassFactory();
		~ClassFactory();
		virtual HRESULT STDMETHODCALLTYPE CreateInstance(_In_opt_ IUnknown *pUnkOuter, _In_ REFIID riid, _COM_Outptr_ void **ppvObject);
		virtual HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock);
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef(void);
		virtual ULONG STDMETHODCALLTYPE Release(void);
	private:
		LONG    m_cRef;
	};
}

//////////////////////////////////////////////////////////////////////////
STDAPI DllRegisterServer(void);
STDAPI DllUnregisterServer(void);
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv);
STDAPI DllCanUnloadNow(void);
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine);
//////////////////////////////////////////////////////////////////////////
__declspec(selectany) extern  LONG		g_cLocks;
__declspec(selectany) extern  HMODULE	g_hModule;




