#pragma once
#include <ddraw.h>
#include "../Common/TinyString.h"
#include "../Common/TinyEvent.h"
#include "TinyImage.h"
#include "TinyTransform.h"

namespace TinyUI
{
	/// <summary>
	/// Í¼Æ¬»­²¼
	/// </summary>
	class TinyImageSurface
	{
		DISALLOW_COPY_AND_ASSIGN(TinyImageSurface);
	public:
		TinyImageSurface(LPDIRECTDRAWSURFACE7 lpDD7);
	};
	class TinySurface
	{
		DISALLOW_COPY_AND_ASSIGN(TinySurface);
	public:
		TinySurface();
		~TinySurface();
		BOOL Initialize(INT cx, INT cy);
		BOOL Render();
	public:
		TinyComPtr<IDirectDrawSurface7> m_ddsPrimary;
		TinyComPtr<IDirectDrawSurface7> m_ddsBack;
		TinyComPtr<IDirectDraw7>		m_dd7;
	};
};



