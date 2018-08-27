#pragma once
#include "DX10Element.h"

namespace DXFramework
{
	class DX10Element2D : public DX10Element
	{
		DECLARE_DYNAMIC(DX10Element2D)
		DISALLOW_COPY_AND_ASSIGN(DX10Element2D)
	public:
		DX10Element2D();
		virtual ~DX10Element2D();
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
		XMFLOAT2	m_size;//Ԫ�ش�С
		XMFLOAT2	m_scale;//���ű���
		XMFLOAT2	m_translate;//ƽ������
	};
}



