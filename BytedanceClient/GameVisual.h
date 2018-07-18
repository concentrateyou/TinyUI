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
	class GameVisual : public DX10Image2D, public IVisual
	{
		DECLARE_DYNAMIC(GameVisual)
		DISALLOW_COPY_AND_ASSIGN(GameVisual)
	public:
		GameVisual(DX10& dx10);
		virtual ~GameVisual();
		LPCSTR	Name() OVERRIDE;
		BOOL	Open() OVERRIDE;
		BOOL	Process() OVERRIDE;
		BOOL	Close() OVERRIDE;
	public:
		void SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
	private:
		DX10&				m_dx10;
		DX10CaptureRunner	m_captureRunner;
		TinyMutex			m_mutes[2];
		BYTE*				m_textures[2];
	};
}


