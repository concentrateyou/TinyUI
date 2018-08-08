#pragma once
#include "DX11Element.h"

namespace DXFramework
{
	class DX11Texture2DElement : public DX11Element
	{
		DECLARE_DYNAMIC(DX11Texture2DElement)
		DISALLOW_COPY_AND_ASSIGN(DX11Texture2DElement)
	public:
		DX11Texture2DElement();
		virtual ~DX11Texture2DElement();
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
		virtual BOOL	DrawImage(DX11& dx11) = 0;
	protected:
		BOOL		m_bFlipH;
		BOOL		m_bFlipV;
		FLOAT		m_angle;
		XMFLOAT2	m_size;//Ԫ�ش�С
		XMFLOAT2	m_scale;//���ű���
		XMFLOAT2	m_translate;//ƽ������
	};
}


