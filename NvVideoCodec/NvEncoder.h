#pragma once
#include "Common/TinyCommon.h"
#include "../Samples/common/inc/dynlink_cuda.h"
#include "../Samples/common/inc/NvHWEncoder.h"
#include <d3d9.h>
#include <d3d10_1.h>
#include <d3d11.h>

#define MAX_ENCODE_QUEUE 32
#define FRAME_QUEUE 240
#define NUM_OF_MVHINTS_PER_BLOCK8x8   4
#define NUM_OF_MVHINTS_PER_BLOCK8x16  2
#define NUM_OF_MVHINTS_PER_BLOCK16x8  2
#define NUM_OF_MVHINTS_PER_BLOCK16x16 1

namespace NVIDIA
{
	typedef enum
	{
		NV_ENC_DX9 = 0,
		NV_ENC_DX11 = 1,
		NV_ENC_CUDA = 2,
		NV_ENC_DX10 = 3,
	} NvEncodeDeviceType;

	/// <summary>
	/// N¿¨±àÂë
	/// </summary>
	class NvEncoder
	{
	public:
		NvEncoder();
		virtual ~NvEncoder();
	private:
		EncodeConfig	encodeConfig;
	};
}


