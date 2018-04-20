#pragma once
#include "DXFramework.h"
#include "DX9Texture2D.h"
#include "Render/TinyRectTracker.h"
using namespace TinyUI;

namespace DXFramework
{
	/// <summary>
	/// DX9Ԫ��
	/// </summary>
	class DX9Element2D : public TinyObject
	{
		DECLARE_DYNAMIC(DX9Element2D)
	public:
		DX9Element2D();
		virtual ~DX9Element2D();
	public:
		TinyString		GetElementName() const;
		void			SetElementName(const TinyString& name);
		FLOAT			GetRotate() const;
		D3DXVECTOR2		GetSize() const;
		D3DXVECTOR2		GetScale() const;
		D3DXVECTOR2		GetTranslate() const;
		BOOL			IsFlipH() const;
		BOOL			IsFlipV() const;
		void			SetSize(const D3DXVECTOR2& size);
		void			SetRotate(FLOAT angle);
		void			SetScale(const D3DXVECTOR2& scale);
		void			SetTranslate(const D3DXVECTOR2& pos);
		void			SetFlipH(BOOL bFlag);
		void			SetFlipV(BOOL bFlag);
	public:
		virtual BOOL	Allocate(DX9& dx9) = 0;
		virtual BOOL	Process(DX9& dx9) = 0;
		virtual void	Deallocate(DX9& dx9) = 0;
	protected:
		BOOL			m_bFlipH;
		BOOL			m_bFlipV;
		FLOAT			m_angle;
		D3DXVECTOR2		m_size;//Ԫ�ش�С
		D3DXVECTOR2		m_scale;//���ű���
		D3DXVECTOR2		m_translate;//ƽ������
		TinyString		m_szElement;
	};
}



