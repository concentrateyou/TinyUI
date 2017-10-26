#include "stdafx.h"
#include "NvEncoder.h"

namespace NVIDIA
{
	NvEncoder::NvEncoder()
	{
		encodeConfig.endFrameIdx = INT_MAX;
		encodeConfig.bitrate = 5000000;
		encodeConfig.rcMode = NV_ENC_PARAMS_RC_CONSTQP;
		encodeConfig.gopLength = NVENC_INFINITE_GOPLENGTH;
		encodeConfig.deviceType = NV_ENC_CUDA;
		encodeConfig.codec = NV_ENC_H264;
		encodeConfig.fps = 25;
		encodeConfig.qp = 28;
		encodeConfig.i_quant_factor = DEFAULT_I_QFACTOR;
		encodeConfig.b_quant_factor = DEFAULT_B_QFACTOR;
		encodeConfig.i_quant_offset = DEFAULT_I_QOFFSET;
		encodeConfig.b_quant_offset = DEFAULT_B_QOFFSET;
		encodeConfig.presetGUID = NV_ENC_PRESET_DEFAULT_GUID;
		encodeConfig.pictureStruct = NV_ENC_PIC_STRUCT_FRAME;
		encodeConfig.inputFormat = NV_ENC_BUFFER_FORMAT_NV12;
	}

	NvEncoder::~NvEncoder()
	{

	}

}
