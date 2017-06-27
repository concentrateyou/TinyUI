#pragma once
#include "../IO/TinyIO.h"
#include "TinyGDI.h"

namespace TinyUI
{
	/// <summary>
	/// ͼƬ��
	///     N=#comp     components
	///       1           grey
	///       2           grey, alpha
	///       3           red, green, blue
	///       4           red, green, blue, alpha
	/// ֻ֧��24��32λ
	/// </summary>

	typedef struct tagIMAGE_INFO
	{
		HBITMAP hBitmap;
		BYTE*	Bits;
		UINT	Delay;
	}IMAGE_INFO;

	class TinyImage
	{
	public:
		TinyImage();
		~TinyImage();
		operator HBITMAP() const;//Ĭ�ϵ�һ֡
		BOOL			IsEmpty() const;
		BOOL			Open(LPCSTR pz);
		BOOL			Open(BYTE* p, DWORD size);
		BOOL			Save(LPCSTR pz);//�����BMP
		size_t			GetCount();
		INT				GetDelay(INT index);
		HBITMAP			GetHBITMAP(INT index);
		BYTE*			GetBits(INT index);
		TinySize		GetSize();
		TinyRectangle	GetRectangle();
		void			Close();
	protected:
		INT						m_cx;
		INT						m_cy;
		INT						m_count;
		HBITMAP					m_hBitmap;
		TinyArray<IMAGE_INFO>	m_images;
	};
}

