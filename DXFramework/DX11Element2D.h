#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "Render/TinyRectTracker.h"
using namespace TinyUI;

namespace DXFramework
{
	/// <summary>
	/// DX11Ԫ��
	/// </summary>
	class DX11Element2D : public TinyObject
	{
		DECLARE_DYNAMIC(DX11Element2D)
	public:
		DX11Element2D();
		virtual ~DX11Element2D();
	public:
		TinyString		GetElementName() const;
		void			SetElementName(const TinyString& name);
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
		virtual BOOL	Process(DX11& dx11) = 0;
	public:
		TinyString		m_szElement;
		BOOL			m_bFlipH;
		BOOL			m_bFlipV;
		FLOAT			m_angle;
		XMFLOAT2		m_size;//Ԫ�ش�С
		XMFLOAT2		m_scale;//���ű���
		XMFLOAT2		m_translate;//ƽ������
	};
}



