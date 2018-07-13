#pragma once
#include "DXFramework.h"
#include "DX9Texture2D.h"
#include "Render/TinyRectTracker.h"
using namespace TinyFramework;

namespace DXFramework
{
	/// <summary>
	/// DX9元素
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
		virtual BOOL	Allocate(DX9& dx9) = 0;
		virtual BOOL	Process(DX9& dx9) = 0;
		virtual void	Deallocate(DX9& dx9) = 0;
	protected:
		BOOL			m_bFlipH;
		BOOL			m_bFlipV;
		FLOAT			m_angle;
		XMFLOAT2		m_size;//元素大小
		XMFLOAT2		m_scale;//缩放比例
		XMFLOAT2		m_translate;//平移坐标
		TinyString		m_szElement;
	};
}



