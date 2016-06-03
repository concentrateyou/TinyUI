#pragma once
#include "../Common/TinyCommon.h"
#include "../Common/TinyCollection.h"
#include "../Render/TinyTheme.h"
#include "../Common/TinyString.h"
#include <vssym32.h>
#include <uxtheme.h>

#pragma comment(lib, "UxTheme.lib")

namespace TinyUI
{
	/// <summary>
	/// Win32Ö÷Ìâ
	/// </summary>
	class TinyTheme
	{
	public:
		TinyTheme();
		~TinyTheme();
		typedef void(WINAPI* OpenThemeData)(HWND hWND, LPCWSTR pszClassList);
		typedef void(WINAPI* CloseThemeData) ();
		typedef HRESULT(WINAPI* DrawThemeBackground) (HDC hDC, INT iPartID, INT iStateID, const RECT *pRect, const RECT* pClipRect);
		typedef HRESULT(WINAPI* DrawThemeText) (HDC hDC, INT iPartID, INT iStateID, LPCWSTR pszText, INT iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT* pRect);
		typedef HRESULT(WINAPI* GetThemeBackgroundContentRect) (HDC hDC, INT iPartID, INT iStateID, const RECT *pBoundingRect, RECT* pContentRect);
		typedef HRESULT(WINAPI* GetThemeBackgroundExtent)  (HDC hDC, INT iPartID, INT iStateID, const RECT *pContentRect, RECT* pExtentRect);
		typedef HRESULT(WINAPI* DrawThemeBackgroundEx)  (HDC hDC, INT iPartID, INT iStateID, const RECT *pRect, const PDTBGOPTS pOptions);
		typedef HRESULT(WINAPI* DrawThemeEdge)  (HDC hDC, INT iPartID, INT iStateID, const RECT *pDestRect, UINT uEdge, UINT uFlags, RECT* pContentRect);
		typedef HRESULT(WINAPI* DrawThemeIcon)  (HDC hDC, INT iPartID, INT iStateID, const RECT* pRect, HIMAGELIST himl, INT iImageIndex);
		typedef HRESULT(WINAPI* DrawThemeParentBackground)  (HWND hWnd, HDC hDC, RECT* pRect);
		typedef HRESULT(WINAPI* EnableThemeDialogTexture)  (HWND hWnd, DWORD dwFlags);
		typedef HRESULT(WINAPI* EnableTheming)  (BOOL bEnable);
		typedef HRESULT(WINAPI* GetCurrentThemeName)  (LPWSTR pszThemeFileName, INT dwMaxNameChars, LPWSTR pszColorBuff, INT cchMaxColorChars, LPWSTR pszSizeBuff, INT cchMaxSizeChars);
		typedef HRESULT(WINAPI* GetThemeBackgroundRegion)  (HDC hDC, INT iPartID, INT iStateID, const RECT *pRect, HRGN* pRegion);
		typedef HRESULT(WINAPI* GetThemeBool)  (INT iPartID, INT iStateID, INT iPropID, BOOL* pfVal);
		typedef HRESULT(WINAPI* GetThemeColor)  (INT iPartID, INT iStateID, INT iPropID, COLORREF* pColor);
		typedef HRESULT(WINAPI* GetThemeDocumentationProperty)  (LPCWSTR pszThemeName, LPCWSTR pszPropertyName, LPWSTR pszValueBuff, INT cchMaxValChars);
		typedef HRESULT(WINAPI* GetThemeEnumValue)  (INT iPartID, INT iStateID, INT iPropID, INT* piVal);
		typedef HRESULT(WINAPI* GetThemeFilename)  (INT iPartID, INT iStateID, INT iPropID, LPWSTR pszThemeFilename, INT cchMaxBuffChars);
		typedef HRESULT(WINAPI* GetThemeFont)  (HDC hDC, INT iPartID, INT iStateID, INT iPropID, LOGFONT* pFont);
		typedef HRESULT(WINAPI* GetThemeInt)  (INT iPartID, INT iStateID, INT iPropID, INT* piVal);
		typedef HRESULT(WINAPI* GetThemeIntList)  (INT iPartID, INT iStateID, INT iPropID, INTLIST* pIntList);
		typedef HRESULT(WINAPI* GetThemeMargins)  (HDC hDC, INT iPartID, INT iStateID, INT iPropID, RECT *pRect, MARGINS* pMargins);
		typedef HRESULT(WINAPI* GetThemeMetric)  (HDC hDC, INT iPartID, INT iStateID, INT iPropID, INT* piVal);
		typedef HRESULT(WINAPI* GetThemePartSize)  (HDC hDC, INT iPartID, INT iStateID, RECT *pRect, THEMESIZE eSize, SIZE* pSize);
		typedef HRESULT(WINAPI* GetThemePosition)  (INT iPartID, INT iStateID, INT iPropID, POINT* pPoint);
		typedef HRESULT(WINAPI* GetThemePropertyOrigin)  (INT iPartID, INT iStateID, INT iPropID, PROPERTYORIGIN* pOrigin);
		typedef HRESULT(WINAPI* GetThemeRect)  (INT iPartID, INT iStateID, INT iPropID, RECT* pRect);
		typedef HRESULT(WINAPI* GetThemeString)  (INT iPartID, INT iStateID, INT iPropID, LPWSTR pszBuff, INT cchMaxBuffChars);
		typedef BOOL(WINAPI* GetThemeSysBool)   (INT iBoolID);
		typedef COLORREF(WINAPI* GetThemeSysColor)  (INT iColorID);
		typedef HBRUSH(WINAPI* GetThemeSysColorBrush)  (INT iColorID);
		typedef HRESULT(WINAPI* GetThemeSysFont) (INT iFontID, LOGFONT* pLF);
		typedef HRESULT(WINAPI* GetThemeSysInt) (INT iIntID, INT* piValue);
		typedef INT(WINAPI* GetThemeSysSize) (INT iSizeID);
		typedef HRESULT(WINAPI* GetThemeSysString)  (INT iStringID, LPWSTR pszStringBuff, INT cchMaxStringChars);
		typedef HRESULT(WINAPI* GetThemeTextExtent)  (HDC hDC, INT iPartID, INT iStateID, LPCWSTR pszText, INT iCharCount, DWORD dwTextFlags, const RECT *pBoundingRect, RECT* pExtentRect);
		typedef HRESULT(WINAPI* GetThemeTextMetrics)  (HDC hDC, INT iPartID, INT iStateID, TEXTMETRIC* ptm);
		typedef HTHEME(WINAPI* GetWindowTheme)  (HWND hWnd);
		typedef HRESULT(WINAPI* HitTestThemeBackground)  (HDC hDC, INT iPartID, INT iStateID, DWORD dwOptions, const RECT *pRect, HRGN hRgn, POINT ptTest, WORD* pwHitTestCode);
		typedef DWORD(WINAPI* ThemeAppProperties) ();
		typedef BOOL(WINAPI* IsAppThemed) ();
		typedef BOOL(WINAPI* IsThemeActive) ();
		typedef BOOL(WINAPI* IsAppThemeReady) ();
		typedef BOOL(WINAPI* IsThemeBackgroundPartiallyTransparent) (INT iPartID, INT iStateID);
		typedef BOOL(WINAPI* IsThemeDialogTextureEnabled) (HWND hWnd);
		typedef BOOL(WINAPI* IsThemePartDefined) (INT iPartID, INT iStateID);
		typedef void(WINAPI* SetThemeAppProperties) (DWORD dwFlags);
		typedef HRESULT(WINAPI* SetWindowTheme) (HWND hWnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
		typedef BOOL(WINAPI* ThemeDataOpen) ();
		typedef DWORD(WINAPI* GetThemeAppProperties) ();
	public:
		void EnumThemeFunctions();
	private:
		TinyMap<TinyString, LPVOID> m_apiMaps;
		HMODULE m_hThemeDLL;
	};
}