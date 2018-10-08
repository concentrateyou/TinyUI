#pragma once
#include "MediaSDK.h"
#include "IVisual2D.h"

namespace MediaSDK
{
	class MonitorVisual2D : public IVisual2D
	{
		DECLARE_DYNAMIC(MonitorVisual2D)
		DISALLOW_COPY_AND_ASSIGN(MonitorVisual2D)
	public:
		MonitorVisual2D(DX11& dx11);
		virtual	~MonitorVisual2D();
	public:
		UINT			GetOutputCount() const;
		BOOL			SetOutput(UINT32 index, BOOL bCursor = TRUE);
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Tick(INT64& timestamp) OVERRIDE;
		BOOL			Draw(DX11Graphics2D& g) OVERRIDE;
		void			Close() OVERRIDE;
		LPCSTR			GetName() OVERRIDE;
		XMFLOAT2		GetScale() OVERRIDE;
		XMFLOAT2		GetTranslate() OVERRIDE;
		XMFLOAT2		GetSize() OVERRIDE;
		void			SetName(LPCSTR pzName) OVERRIDE;
		void			SetTranslate(const XMFLOAT2& pos) OVERRIDE;
		void			SetScale(const XMFLOAT2& pos) OVERRIDE;
	private:
		BOOL			m_bCursor;
		UINT32			m_index;
		TinyString		m_szNAME;
		DX11&			m_dx11;
		DX11Cursor2D	m_cursor2D;
		DX11Image2D		m_visual2D;
		DX11Duplicator	m_duplicator;
	};
}



