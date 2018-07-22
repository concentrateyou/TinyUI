#pragma once
#include "DX10Element.h"

namespace DXFramework
{
	class DX10Texture2DElement : public DX10Element
	{
		DECLARE_DYNAMIC(DX10Texture2DElement)
		DISALLOW_COPY_AND_ASSIGN(DX10Texture2DElement)
	public:
		DX10Texture2DElement();
		virtual ~DX10Texture2DElement();
	public:
		FLOAT			GetRotate() const;
		XMFLOAT2		GetSize() const;
		XMFLOAT2		GetScale() const;
		XMFLOAT2		GetTranslate() const;
		BOOL			IsFlipH() const;
		BOOL			IsFlipV() const;
		void			SetSize(const XMFLOAT2& size);
		void			SetRotate(FLOAT angle);
		void			SetScale(const XMFLOAT2& scale);
		void			SetTranslate(const XMFLOAT2& pos);
		void			SetFlipH(BOOL bFlag);
		void			SetFlipV(BOOL bFlag);
	public:
		virtual BOOL	DrawImage(DX10& dx10) = 0;
	protected:
		BOOL		m_bFlipH;
		BOOL		m_bFlipV;
		FLOAT		m_angle;
		XMFLOAT2	m_size;//元素大小
		XMFLOAT2	m_scale;//缩放比例
		XMFLOAT2	m_translate;//平移坐标
	};
}



