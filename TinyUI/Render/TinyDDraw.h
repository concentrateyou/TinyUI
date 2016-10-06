#pragma once
#include <ddraw.h>
#include "../Common/TinyString.h"
#include "../Common/TinyEvent.h"
#include "TinyImage.h"
#include "TinyTransform.h"
#pragma comment(lib,"ddraw.lib")
#pragma comment(lib,"dxguid.lib")

namespace TinyUI
{
	class TinyImageSurface
	{
	public:
		TinyImageSurface();
		BOOL Create(LPDIRECTDRAW7 pDD7, INT cx, INT cy);
		BOOL LoadImage(LPDIRECTDRAW7 pDD7, LPCSTR pzFile);
		BOOL LoadImage(LPDIRECTDRAW7 pDD7, BYTE* p, DWORD dwSize);
		BOOL Fill(LPVOID ps, DWORD dwSize);
		BOOL Draw(LPDIRECTDRAWSURFACE7 lpDest, INT destX, INT destY, INT srcX, INT srcY, INT srcCX, INT srcCY);
		LPDIRECTDRAWSURFACE7 GetSurface() const;
	public:
		TinyComPtr<IDirectDrawSurface7> m_dds;
	private:
		TinyImage	m_image;
		TinySize	m_size;
	};

	class TinySurface
	{
		DISALLOW_COPY_AND_ASSIGN(TinySurface)
	public:
		TinySurface();
		~TinySurface();
		BOOL Create();
		void Render();
		LPDIRECTDRAW7 GetDirectDraw() const;
	public:
		TinyComPtr<IDirectDrawSurface7> m_ddsPrimary;
		TinyComPtr<IDirectDraw7>		m_dd7;
	};
};



