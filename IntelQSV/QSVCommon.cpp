#include "stdafx.h"
#include "QSVCommon.h"

namespace QSV
{
	mfxU32 GetIntelAdapter(mfxSession session)
	{
		mfxU32  adapterNum = 0;
		mfxIMPL impl;
		MFXQueryIMPL(session, &impl);
		mfxIMPL baseImpl = MFX_IMPL_BASETYPE(impl);
		for (mfxU8 i = 0; i < sizeof(implTypes) / sizeof(implTypes[0]); i++)
		{
			if (implTypes[i].impl == baseImpl) {
				adapterNum = implTypes[i].adapterID;
				break;
			}
		}
		return adapterNum;
	}
}