#include "stdafx.h"
#include "NVCodec.h"

namespace NVCodec
{
	BOOL checkCudaCapabilitiesDRV(INT majorV, INT minorV, INT devID)
	{
		CUdevice cuDevice;
		CHAR name[256];
		INT major = 0, minor = 0;
		CUresult cuResult = CUDA_SUCCESS;
		cuResult = cuDeviceGet(&cuDevice, devID);
		if (cuResult != CUDA_SUCCESS)
			return FALSE;
		cuResult = cuDeviceGetName(name, 256, cuDevice);
		if (cuResult != CUDA_SUCCESS)
			return FALSE;
		cuResult = cuDeviceComputeCapability(&major, &minor, devID);
		return (major > majorV) || (major == majorV && minor >= minorV);
	}
}
