#pragma once
#include "../IO/TinyIO.h"
#include "TinyGDI.h"

namespace TinyUI
{
	class TinyImage
	{
		DISALLOW_COPY_AND_ASSIGN(TinyImage)
	public:
		TinyImage();
		~TinyImage();
		operator HBITMAP() const;//Ĭ�ϵ�һ֡
		BOOL			IsEmpty() const;
		BOOL			Open(LPCSTR pz);
		BOOL			Open(BYTE* p, DWORD size);
		BOOL			Save(LPCSTR pz);//�����BMP
		HBITMAP			GetHBITMAP();
		BYTE*			GetBits();
		TinySize		GetSize();
		TinyRectangle	GetRectangle();
		void			Close();
	private:
		BOOL			OpenFile(LPCSTR pzFile);
	protected:
		INT				m_cx;
		INT				m_cy;
		BYTE*			m_bits;
		HBITMAP			m_hBitmap;
	};
}

