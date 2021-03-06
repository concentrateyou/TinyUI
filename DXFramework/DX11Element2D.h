#pragma once
#include "DX11Element.h"

namespace DXFramework
{
	class DX11Element2D : public DX11Element
	{
		DECLARE_DYNAMIC(DX11Element2D)
		DISALLOW_COPY_AND_ASSIGN(DX11Element2D)
	public:
		DX11Element2D();
		virtual ~DX11Element2D();
	public:
		FLOAT				GetRotate() const;
		XMFLOAT2			GetSize() const;
		XMFLOAT2			GetScale() const;
		XMFLOAT2			GetTranslate() const;
		VideoPixelFormat	GetFormat() const;
		BOOL				IsFlipH() const;
		BOOL				IsFlipV() const;
		void				SetSize(const XMFLOAT2& size);
		void				SetRotate(FLOAT angle);
		void				SetScale(const XMFLOAT2& scale);
		void				SetTranslate(const XMFLOAT2& pos);
		void				SetFlipH(BOOL bFlag);
		void				SetFlipV(BOOL bFlag);
	public:
		virtual BOOL		DrawImage(DX11& dx11) = 0;
	protected:
		BOOL				m_bFlipH;
		BOOL				m_bFlipV;
		VideoPixelFormat	m_videoF;
		FLOAT				m_angle;
		XMFLOAT2			m_size;//元素大小
		XMFLOAT2			m_scale;//缩放比例
		XMFLOAT2			m_translate;//平移坐标
	};
}



