#include "stdafx.h"
#include "NVDecoder.h"

namespace NVCodec
{
	NVDecoder::NVDecoder()
		:m_decoder(NULL),
		m_parser(NULL),
		m_videolock(NULL),
		m_context(NULL),
		m_bInterop(TRUE),
		m_deviceID(0)
	{
		ZeroMemory(&m_createINFO, sizeof(m_createINFO));
		ZeroMemory(&m_videoFMT, sizeof(m_videoFMT));
	}

	NVDecoder::~NVDecoder()
	{
	}

	BOOL NVDecoder::Open(const TinySize& targetSize, const BYTE* bits, LONG size)
	{
		CUDADRIVER hHandleDriver = 0;
		CUresult cuResult = CUDA_SUCCESS;
		cuResult = cuInit(0, __CUDA_API_VERSION, hHandleDriver);
		if (cuResult != CUDA_SUCCESS)
			goto _ERROR;
		cuResult = cuvidInit(0);
		if (cuResult != CUDA_SUCCESS)
			goto _ERROR;
		return TRUE;
	_ERROR:
		return FALSE;
	}
	//BOOL NVDecoder::InitializeD3D9()
	//{
	//	HRESULT hRes = S_OK;
	//	CUresult cuResult = CUDA_SUCCESS;
	//	INT count = 0;
	//	INT dev = 0;
	//	cuResult = cuDeviceGetCount(&count);
	//	if (cuResult != CUDA_SUCCESS)
	//		return FALSE;
	//	if (count <= 0)
	//		return FALSE;
	//	//找默认第一个
	//	for (INT i = 0; i < count; i++)
	//	{
	//		cuResult = cuDeviceGet(&dev, i);
	//		if (cuResult != CUDA_SUCCESS)
	//			return FALSE;
	//		if (!checkCudaCapabilitiesDRV(1, 1, i))
	//			return FALSE;
	//		INT TCC = 0;
	//		cuResult = cuDeviceGetAttribute(&TCC, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, dev);
	//		if (cuResult != CUDA_SUCCESS)
	//			return FALSE;
	//		if (TCC > 0)
	//		{
	//			m_bInterop = FALSE;
	//			continue;
	//		}
	//		else
	//		{
	//			m_deviceID = i;
	//			break;
	//		}
	//	}
	//	if (m_bInterop)
	//	{
	//		m_d3d9.Attach(Direct3DCreate9(D3D_SDK_VERSION));
	//		if (!m_d3d9)
	//			return FALSE;
	//		BOOL bF = FALSE;
	//		CUdevice device = 0;
	//		D3DADAPTER_IDENTIFIER9 adapterID;
	//		for (UINT i = 0; i < m_d3d9->GetAdapterCount(); i++)
	//		{
	//			hRes = m_d3d9->GetAdapterIdentifier(i, 0, &adapterID);
	//			if (FAILED(hRes))
	//				continue;
	//			cuResult = cuD3D9GetDevice(&device, adapterID.DeviceName);
	//			if (cuResult != CUDA_SUCCESS)
	//				return FALSE;
	//			else
	//			{
	//			}
	//		}
	//	}
	//}
	BOOL NVDecoder::Decode(Media::SampleTag& tag)
	{
		CUVIDSOURCEDATAPACKET packet = { 0 };
		packet.payload_size = tag.size;
		packet.payload = tag.bits;
		packet.timestamp = static_cast<CUvideotimestamp>(tag.timestamp);
		return TRUE;
	}
	void NVDecoder::Close()
	{

	}
}
