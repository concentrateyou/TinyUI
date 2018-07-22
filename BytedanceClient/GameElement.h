#pragma once
#include "Common/TinyModule.h"
#include "IVisual.h"
#include "DX10.h"
#include "DX10CaptureRunner.h"
#include "DX10Image2D.h"
using namespace TinyFramework;
using namespace DXFramework;

namespace Bytedance
{
	/// <summary>
	/// 游戏可视化
	/// </summary>
	class GameElement : public DX10Image2D, public IVisual
	{
		DECLARE_DYNAMIC(GameElement)
		DISALLOW_COPY_AND_ASSIGN(GameElement)
	public:
		GameElement(DX10& dx10);
		virtual ~GameElement();
		void			Select(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Process() OVERRIDE;
		BOOL			Close() OVERRIDE;
		LPCSTR			GetName() OVERRIDE;
		DX11Element*	GetVisual() OVERRIDE;
	private:
		DX10&				m_dx10;
		DX10CaptureRunner	m_captureRunner;
		TinyMutex			m_mutes[2];
		BYTE*				m_textures[2];
	};
}


