#pragma once
#include "TinyControl.h"

namespace TinyUI
{
	enum SkinAPI
	{
		SkinAPILoadLibraryA,
		SkinAPILoadLibraryW,
		SkinAPILoadLibraryExA,
		SkinAPILoadLibraryExW,
		SkinAPIGetProcAddress,
		SkinAPIGetModuleHandleA,
		SkinAPIRegisterClassA,
		SkinAPIRegisterClassW,
		SkinAPIGetSysColor,
		SkinAPIGetSysColorBrush,
		SkinAPIDrawEdge,
		SkinAPIDrawFrameControl,
		SkinAPISetScrollInfo,
		SkinAPISetScrollPos,
		SkinAPIGetScrollInfo,
		SkinAPIEnableScrollBar,
		SkinAPIOpenThemeData,
		SkinAPIDrawThemeBackground,
		SkinAPICloseThemeData,
		SkinAPIGetThemeColor,
		SkinAPIGetThemeInt,
		SkinAPIIsAppThemed,
		SkinAPIIsThemeActive,
		SkinAPIGetCurrentThemeName,
		SkinAPIGetThemeSysBool,
		SkinAPIGetThemeSysColor,
		SkinAPIGetThemePartSize,
		SkinAPISystemParametersInfo,
		SkinAPIDefWindowProcA,
		SkinAPIDefWindowProcW,
		SkinAPIDefMDIChildProcA,
		SkinAPIDefMDIChildProcW,
		SkinAPIDefFrameProcA,
		SkinAPIDefFrameProcW,
		SkinAPIDefDlgProcA,
		SkinAPIDefDlgProcW,
		SkinAPICallWindowProcA,
		SkinAPICallWindowProcW,
		SkinAPICreateThread,
		SkinAPIFillRect,
		SkinAPIDeleteObject,
		SkinAPIAdjustWindowRectEx,
		SkinAPIAdjustWindowRect,
		SkinAPIGetSystemMetrics,
		SkinAPITrackPopupMenu,
		SkinAPITrackPopupMenuEx,
		SkinAPIDrawMenuBar,
		SkinAPILoadLibraryEx2W,
		SKIN_APIHOOKCOUNT
	};
	/// <summary>
	/// ∆§∑ÙHook
	/// </summary>
	class TinySkinAPIHook
	{
	public:
		TinySkinAPIHook();
		virtual ~TinySkinAPIHook();
	public:
		//static WINAPI TinySkinAPIHook* GetInstance();
	};
	/// <summary>
	/// ∆§∑ÙAPI
	/// </summary>
	class TinySkinAPI
	{
	public:
		TinySkinAPI(TinySkinAPIHook* pHook, LPCSTR pszCalleeModName, LPCSTR pszFuncName, PROC pfnOrig, PROC pfnHook);
		virtual ~TinySkinAPI();
	public:
		BOOL BeginHook();
		BOOL EndHook();
	};
}
