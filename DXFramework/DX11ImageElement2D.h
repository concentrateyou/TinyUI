#pragma once
#include "DX11Element2D.h"

namespace DXFramework
{
	class DX11ImageElement2D : public DX11Element2D
	{
		DECLARE_DYNAMIC(DX11ImageElement2D)
		DISALLOW_COPY_AND_ASSIGN(DX11ImageElement2D)
	public:
		DX11ImageElement2D();
		virtual ~DX11ImageElement2D();
	public:
		FLOAT		GetRotate() const;
		XMFLOAT2	GetSize() const;
		XMFLOAT2	GetScale() const;
		XMFLOAT2	GetTranslate() const;
		BOOL		IsFlipH() const;
		BOOL		IsFlipV() const;
		void		SetSize(const XMFLOAT2& size);
		void		SetRotate(FLOAT angle);
		void		SetScale(const XMFLOAT2& scale);
		void		SetTranslate(const XMFLOAT2& pos);
		void		SetFlipH(BOOL bFlag);
		void		SetFlipV(BOOL bFlag);
	protected:
		BOOL		m_bFlipH;
		BOOL		m_bFlipV;
		FLOAT		m_angle;
		XMFLOAT2	m_size;//元素大小
		XMFLOAT2	m_scale;//缩放比例
		XMFLOAT2	m_translate;//平移坐标
	};
}



