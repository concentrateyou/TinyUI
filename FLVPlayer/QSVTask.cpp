#include "stdafx.h"
#include "QSVTask.h"
#include "QSVDecoder.h"

namespace FLVPlayer
{
	QSVTask::QSVTask()
		:m_hFile(NULL),
		m_pNV12(NULL)
	{

	}
	QSVTask::~QSVTask()
	{
		fclose(m_hFile);
	}
	BOOL QSVTask::Initialize(HWND hWND)
	{
		if (!m_reader.OpenFile("D:\\3.flv"))
			return FALSE;
		TinySize size((static_cast<LONG>(m_reader.GetScript().width)), (static_cast<LONG>(m_reader.GetScript().height)));
		TinyRectangle rectangle;
		::GetClientRect(hWND, &rectangle);
		if (!m_graphics.Initialize(hWND, rectangle.Size()))
			return FALSE;
		if (!m_video2D.Create(m_graphics.GetDX11(), size, FALSE))
			return FALSE;
		m_video2D.SetScale(rectangle.Size());

		m_pNV12 = av_frame_alloc();
		if (!m_pNV12)
			return FALSE;
		INT val = av_image_get_buffer_size(AV_PIX_FMT_NV12, size.cx, size.cy, 1);
		m_bits.Reset(new BYTE[val]);
		ZeroMemory(m_bits, val);
		av_image_fill_arrays(m_pNV12->data, m_pNV12->linesize, m_bits.Ptr(), AV_PIX_FMT_NV12, size.cx, size.cy, 1);
		return TRUE;
	}
	BOOL QSVTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&QSVTask::OnMessagePump, this));
	}
	BOOL QSVTask::Close(DWORD dwMS)
	{
		return TinyTaskBase::Close(dwMS);
	}

	mfxStatus QSVTask::OnInvoke()
	{
		MSDK_FOPEN(m_hFile, "D:\\test1.yuv", "wb");
		MSDK_CHECK_POINTER(m_hFile, MFX_ERR_NULL_PTR);
		mfxStatus status = MFX_ERR_NONE;
		FLV_BLOCK block = { 0 };
		TinySize size(static_cast<LONG>(m_reader.GetScript().width), static_cast<LONG>(m_reader.GetScript().height));
		for (;;)
		{
			if (!m_reader.ReadBlock(block))
				break;
			if (block.type == FLV_VIDEO)
			{
				if (block.video.codeID == FLV_CODECID_H264)
				{
					if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
					{
						m_qsv.Open(block.video.data, block.video.size);
						//m_x264.Initialize(size, size);
						//m_x264.Open(block.video.data, block.video.size);
					}
					if (block.video.packetType == FLV_NALU)
					{
						Media::SampleTag tag;
						tag.bits = block.video.data;
						tag.size = block.video.size;
						tag.samplePTS = block.pts;
						tag.sampleDTS = block.dts;
						mfxFrameSurface1* video = NULL;
						if (m_qsv.Decode(tag, video))
						{
							m_video2D.Copy(m_graphics.GetDX11(), NULL, video->Data.B, video->Info.CropH * video->Data.Pitch);
							m_graphics.GetDX11().SetRenderTexture2D(NULL);
							m_graphics.GetDX11().GetRender2D()->BeginDraw();
							m_graphics.DrawImage(&m_video2D, 1.0F, 1.0F);
							m_graphics.GetDX11().GetRender2D()->EndDraw();
							m_graphics.Present();
						}
					}
					SAFE_DELETE_ARRAY(block.audio.data);
					SAFE_DELETE_ARRAY(block.video.data);
					Sleep(30);
				}
			}
		}

		MSDK_IGNORE_MFX_STS(status, MFX_ERR_MORE_DATA);
		MSDK_CHECK_RESULT(status, MFX_ERR_NONE, status);
		return status;
	}

	void QSVTask::OnMessagePump()
	{
		//OnMessagePump2();
		//OnMessagePump1();
		OnInvoke();
	}

	mfxStatus QSVTask::OnMessagePump1()
	{
		FILE* fSource;
		MSDK_FOPEN(fSource, "D:\\test1.h264", "rb");
		MSDK_CHECK_POINTER(fSource, MFX_ERR_NULL_PTR);

		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		MFXVideoSession session;
		mfxStatus sts = ::Initialize(impl, ver, &session, NULL);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// Create Media SDK decoder
		MFXVideoDECODE mfxDEC(session);
		// Create Media SDK VPP component
		MFXVideoVPP mfxVPP(session);

		// Set required video parameters for decode
		mfxVideoParam mfxVideoParams;
		memset(&mfxVideoParams, 0, sizeof(mfxVideoParams));
		mfxVideoParams.mfx.CodecId = MFX_CODEC_AVC;
		mfxVideoParams.IOPattern = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

		// Prepare Media SDK bit stream buffer
		// - Arbitrary buffer size for this example
		mfxBitstream mfxBS;
		memset(&mfxBS, 0, sizeof(mfxBS));
		mfxBS.MaxLength = 1024 * 1024;
		mfxBS.Data = new mfxU8[mfxBS.MaxLength];
		MSDK_CHECK_POINTER(mfxBS.Data, MFX_ERR_MEMORY_ALLOC);

		// Read a chunk of data from stream file into bit stream buffer
		// - Parse bit stream, searching for header and fill video parameters structure
		// - Abort if bit stream header is not found in the first bit stream buffer chunk
		sts = ReadBitStreamData(&mfxBS, fSource);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		sts = mfxDEC.DecodeHeader(&mfxBS, &mfxVideoParams);
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// Initialize VPP parameters
		// - For simplistic memory management, system memory surfaces are used to store the raw frames
		//   (Note that when using HW acceleration D3D surfaces are prefered, for better performance)
		mfxVideoParam VPPParams;
		memset(&VPPParams, 0, sizeof(VPPParams));
		// Input data
		VPPParams.vpp.In.FourCC = MFX_FOURCC_NV12;
		VPPParams.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		VPPParams.vpp.In.CropX = 0;
		VPPParams.vpp.In.CropY = 0;
		VPPParams.vpp.In.CropW = mfxVideoParams.mfx.FrameInfo.CropW;
		VPPParams.vpp.In.CropH = mfxVideoParams.mfx.FrameInfo.CropH;
		VPPParams.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		VPPParams.vpp.In.FrameRateExtN = 30;
		VPPParams.vpp.In.FrameRateExtD = 1;
		// width must be a multiple of 16
		// height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
		VPPParams.vpp.In.Width = MSDK_ALIGN16(VPPParams.vpp.In.CropW);
		VPPParams.vpp.In.Height = (MFX_PICSTRUCT_PROGRESSIVE == VPPParams.vpp.In.PicStruct) ? MSDK_ALIGN16(VPPParams.vpp.In.CropH) : MSDK_ALIGN32(VPPParams.vpp.In.CropH);
		// Output data
		VPPParams.vpp.Out.FourCC = MFX_FOURCC_RGB4;
		VPPParams.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		VPPParams.vpp.Out.CropX = 0;
		VPPParams.vpp.Out.CropY = 0;
		VPPParams.vpp.Out.CropW = VPPParams.vpp.In.CropW;   // Resize to half size resolution
		VPPParams.vpp.Out.CropH = VPPParams.vpp.In.CropH;
		VPPParams.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		VPPParams.vpp.Out.FrameRateExtN = 30;
		VPPParams.vpp.Out.FrameRateExtD = 1;
		// width must be a multiple of 16
		// height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
		VPPParams.vpp.Out.Width = MSDK_ALIGN16(VPPParams.vpp.Out.CropW);
		VPPParams.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == VPPParams.vpp.Out.PicStruct) ? MSDK_ALIGN16(VPPParams.vpp.Out.CropH) : MSDK_ALIGN32(VPPParams.vpp.Out.CropH);

		VPPParams.IOPattern = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

		// Query number of required surfaces for decoder
		mfxFrameAllocRequest DecRequest;
		memset(&DecRequest, 0, sizeof(DecRequest));
		sts = mfxDEC.QueryIOSurf(&mfxVideoParams, &DecRequest);
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// Query number of required surfaces for VPP
		mfxFrameAllocRequest VPPRequest[2];     // [0] - in, [1] - out
		memset(&VPPRequest, 0, sizeof(mfxFrameAllocRequest) * 2);
		sts = mfxVPP.QueryIOSurf(&VPPParams, VPPRequest);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// Determine the required number of surfaces for decoder output (VPP input) and for VPP output
		mfxU16 nSurfNumDecVPP = DecRequest.NumFrameSuggested + VPPRequest[0].NumFrameSuggested;
		mfxU16 nSurfNumVPPOut = VPPRequest[1].NumFrameSuggested;

		// Allocate surfaces for decoder and VPP In
		// - Width and height of buffer must be aligned, a multiple of 32
		// - Frame surface array keeps pointers all surface planes and general frame info
		mfxU16 width = (mfxU16)MSDK_ALIGN32(DecRequest.Info.Width);
		mfxU16 height = (mfxU16)MSDK_ALIGN32(DecRequest.Info.Height);
		mfxU8 bitsPerPixel = 12;        // NV12 format is a 12 bits per pixel format
		mfxU32 surfaceSize = width * height * bitsPerPixel / 8;
		mfxU8* surfaceBuffers = (mfxU8*) new mfxU8[surfaceSize * nSurfNumDecVPP];

		mfxFrameSurface1** pmfxSurfaces = new mfxFrameSurface1 *[nSurfNumDecVPP];
		MSDK_CHECK_POINTER(pmfxSurfaces, MFX_ERR_MEMORY_ALLOC);
		for (int i = 0; i < nSurfNumDecVPP; i++) {
			pmfxSurfaces[i] = new mfxFrameSurface1;
			memset(pmfxSurfaces[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(pmfxSurfaces[i]->Info), &(mfxVideoParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
			pmfxSurfaces[i]->Data.Y = &surfaceBuffers[surfaceSize * i];
			pmfxSurfaces[i]->Data.U = pmfxSurfaces[i]->Data.Y + width * height;
			pmfxSurfaces[i]->Data.V = pmfxSurfaces[i]->Data.U + 1;
			pmfxSurfaces[i]->Data.Pitch = width;
		}

		// Allocate surfaces for VPP Out
		// - Width and height of buffer must be aligned, a multiple of 32
		// - Frame surface array keeps pointers all surface planes and general frame info
		width = (mfxU16)(VPPRequest[1].Info.Width);
		height = (mfxU16)(VPPRequest[1].Info.Height);
		bitsPerPixel = 32;
		surfaceSize = width * height * bitsPerPixel / 8;
		mfxU8* surfaceBuffers2 = (mfxU8*) new mfxU8[surfaceSize * nSurfNumVPPOut];

		mfxFrameSurface1** pmfxSurfaces2 = new mfxFrameSurface1 *[nSurfNumVPPOut];
		MSDK_CHECK_POINTER(pmfxSurfaces2, MFX_ERR_MEMORY_ALLOC);
		for (int i = 0; i < nSurfNumVPPOut; i++) {
			pmfxSurfaces2[i] = new mfxFrameSurface1;
			memset(pmfxSurfaces2[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(pmfxSurfaces2[i]->Info), &(VPPParams.vpp.Out), sizeof(mfxFrameInfo));

			pmfxSurfaces2[i]->Data.B = &surfaceBuffers[surfaceSize * i];
			pmfxSurfaces2[i]->Data.G = pmfxSurfaces2[i]->Data.B + 1;
			pmfxSurfaces2[i]->Data.R = pmfxSurfaces2[i]->Data.B + 2;
			pmfxSurfaces2[i]->Data.A = pmfxSurfaces2[i]->Data.B + 3;

			//pmfxSurfaces2[i]->Data.R = &surfaceBuffers[surfaceSize * i];
			//pmfxSurfaces2[i]->Data.G = pmfxSurfaces2[i]->Data.R + 1;
			//pmfxSurfaces2[i]->Data.B = pmfxSurfaces2[i]->Data.R + 2;
			//pmfxSurfaces2[i]->Data.A = pmfxSurfaces2[i]->Data.R + 3;
			pmfxSurfaces2[i]->Data.Pitch = width * 4;
		}

		// Initialize the Media SDK decoder
		sts = mfxDEC.Init(&mfxVideoParams);
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// Initialize Media SDK VPP
		sts = mfxVPP.Init(&VPPParams);
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// ===============================================================
		// Start decoding the frames from the stream
		//
		mfxTime tStart, tEnd;
		mfxGetTime(&tStart);

		mfxSyncPoint syncpD;
		mfxSyncPoint syncpV;
		mfxFrameSurface1* pmfxOutSurface = NULL;
		int nIndex = 0;
		int nIndex2 = 0;
		mfxU32 nFrame = 0;

		//
		// Stage 1: Main decoding loop
		//
		while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_DATA == sts || MFX_ERR_MORE_SURFACE == sts)
		{
			if (MFX_WRN_DEVICE_BUSY == sts)
				MSDK_SLEEP(1);  // Wait if device is busy, then repeat the same call to DecodeFrameAsync

			if (MFX_ERR_MORE_DATA == sts)
			{
				sts = ReadBitStreamData(&mfxBS, fSource);       // Read more data into input bit stream
				MSDK_BREAK_ON_ERROR(sts);
			}

			if (MFX_ERR_MORE_SURFACE == sts || MFX_ERR_NONE == sts)
			{
				nIndex = GetFreeSurfaceIndex(pmfxSurfaces, nSurfNumDecVPP);     // Find free frame surface
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nIndex, MFX_ERR_MEMORY_ALLOC);
			}
			// Decode a frame asychronously (returns immediately)
			sts = mfxDEC.DecodeFrameAsync(&mfxBS, pmfxSurfaces[nIndex], &pmfxOutSurface, &syncpD);

			// Ignore warnings if output is available,
			// if no output and no action required just repeat the DecodeFrameAsync call
			if (MFX_ERR_NONE < sts && syncpD)
				sts = MFX_ERR_NONE;

			if (MFX_ERR_NONE == sts)
			{
				nIndex2 = GetFreeSurfaceIndex(pmfxSurfaces2, nSurfNumVPPOut);   // Find free frame surface
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nIndex2, MFX_ERR_MEMORY_ALLOC);

				for (;;)
				{
					// Process a frame asychronously (returns immediately)
					sts = mfxVPP.RunFrameVPPAsync(pmfxOutSurface, pmfxSurfaces2[nIndex2], NULL, &syncpV);

					if (MFX_ERR_NONE < sts && !syncpV)
					{    // repeat the call if warning and no output
						if (MFX_WRN_DEVICE_BUSY == sts)
							MSDK_SLEEP(1);  // wait if device is busy
					}
					else if (MFX_ERR_NONE < sts && syncpV)
					{
						sts = MFX_ERR_NONE;     // ignore warnings if output is available
						break;
					}
					else
						break;  // not a warning
				}

				// VPP needs more data, let decoder decode another frame as input
				if (MFX_ERR_MORE_DATA == sts)
				{
					continue;
				}
				else if (MFX_ERR_MORE_SURFACE == sts)
				{
					// Not relevant for the illustrated workload! Therefore not handled.
					// Relevant for cases when VPP produces more frames at output than consumes at input. E.g. framerate conversion 30 fps -> 60 fps
					break;
				}
				else
					MSDK_BREAK_ON_ERROR(sts);
			}

			if (MFX_ERR_NONE == sts)
				sts = session.SyncOperation(syncpV, 60000);     // Synchronize. Wait until decoded frame is ready

			if (MFX_ERR_NONE == sts)
			{
				++nFrame;

				m_video2D.Copy(m_graphics.GetDX11(), NULL, pmfxSurfaces2[nIndex2]->Data.B, pmfxSurfaces2[nIndex2]->Info.Height *  pmfxSurfaces2[nIndex2]->Info.Width * 4);
				m_graphics.GetDX11().SetRenderTexture2D(NULL);
				m_graphics.GetDX11().GetRender2D()->BeginDraw();
				m_graphics.DrawImage(&m_video2D, 1.0F, 1.0F);
				m_graphics.GetDX11().GetRender2D()->EndDraw();
				m_graphics.Present();

				Sleep(15);
			}
		}

		// MFX_ERR_MORE_DATA means that file has ended, need to go to buffering loop, exit in case of other errors
		MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		//
		// Stage 2: Retrieve the buffered decoded frames
		//
		while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_SURFACE == sts)
		{
			if (MFX_WRN_DEVICE_BUSY == sts)
				MSDK_SLEEP(1);  // Wait if device is busy, then repeat the same call to DecodeFrameAsync

			nIndex = GetFreeSurfaceIndex(pmfxSurfaces, nSurfNumDecVPP);     // Find free frame surface
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nIndex, MFX_ERR_MEMORY_ALLOC);

			// Decode a frame asychronously (returns immediately)
			sts = mfxDEC.DecodeFrameAsync(NULL, pmfxSurfaces[nIndex], &pmfxOutSurface, &syncpD);

			// Ignore warnings if output is available,
			// if no output and no action required just repeat the DecodeFrameAsync call
			if (MFX_ERR_NONE < sts && syncpD)
				sts = MFX_ERR_NONE;

			if (MFX_ERR_NONE == sts)
			{
				nIndex2 = GetFreeSurfaceIndex(pmfxSurfaces2, nSurfNumVPPOut);   // Find free frame surface
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nIndex2, MFX_ERR_MEMORY_ALLOC);

				for (;;)
				{
					// Process a frame asychronously (returns immediately)
					sts = mfxVPP.RunFrameVPPAsync(pmfxOutSurface, pmfxSurfaces2[nIndex2], NULL, &syncpV);

					if (MFX_ERR_NONE < sts && !syncpV)
					{    // repeat the call if warning and no output
						if (MFX_WRN_DEVICE_BUSY == sts)
							MSDK_SLEEP(1);  // wait if device is busy
					}
					else if (MFX_ERR_NONE < sts && syncpV)
					{
						sts = MFX_ERR_NONE;     // ignore warnings if output is available
						break;
					}
					else
						break;  // not a warning
				}

				// VPP needs more data, let decoder decode another frame as input
				if (MFX_ERR_MORE_DATA == sts)
				{
					continue;
				}
				else if (MFX_ERR_MORE_SURFACE == sts)
				{
					// Not relevant for the illustrated workload! Therefore not handled.
					// Relevant for cases when VPP produces more frames at output than consumes at input. E.g. framerate conversion 30 fps -> 60 fps
					break;
				}
				else
					MSDK_BREAK_ON_ERROR(sts);
			}

			if (MFX_ERR_NONE == sts)
				sts = session.SyncOperation(syncpV, 60000);     // Synchronize. Waits until decoded frame is ready

			if (MFX_ERR_NONE == sts)
			{
				++nFrame;

			}
		}

		// MFX_ERR_MORE_DATA means that decoder is done with buffered frames, need to go to VPP buffering loop, exit in case of other errors
		MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		//
		// Stage 3: Retrieve the buffered VPP frames
		//
		while (MFX_ERR_NONE <= sts)
		{
			nIndex2 = GetFreeSurfaceIndex(pmfxSurfaces2, nSurfNumVPPOut);   // Find free frame surface
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nIndex2, MFX_ERR_MEMORY_ALLOC);

			// Process a frame asychronously (returns immediately)
			sts = mfxVPP.RunFrameVPPAsync(NULL, pmfxSurfaces2[nIndex2], NULL, &syncpV);
			MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_SURFACE);
			MSDK_BREAK_ON_ERROR(sts);

			sts = session.SyncOperation(syncpV, 60000);     // Synchronize. Wait until frame processing is ready
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

			++nFrame;

		}

		// MFX_ERR_MORE_DATA indicates that all buffers has been fetched, exit in case of other errors
		MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	}

	mfxStatus QSVTask::OnMessagePump2()
	{
		FILE* fSource;
		MSDK_FOPEN(fSource, "D:\\test1.h264", "rb");
		MSDK_CHECK_POINTER(fSource, MFX_ERR_NULL_PTR);

		mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
		mfxVersion ver = { { 0, 1 } };
		MFXVideoSession session;
		mfxFrameAllocator mfxAllocator;
		mfxStatus sts = ::Initialize(impl, ver, &session, &mfxAllocator);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// Create Media SDK decoder
		MFXVideoDECODE mfxDEC(session);
		// Create Media SDK VPP component
		MFXVideoVPP mfxVPP(session);

		// Set required video parameters for decode
		mfxVideoParam mfxVideoParams;
		memset(&mfxVideoParams, 0, sizeof(mfxVideoParams));
		mfxVideoParams.mfx.CodecId = MFX_CODEC_AVC;
		mfxVideoParams.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;

		// Prepare Media SDK bit stream buffer
		// - Arbitrary buffer size for this example
		mfxBitstream mfxBS;
		memset(&mfxBS, 0, sizeof(mfxBS));
		mfxBS.MaxLength = 1024 * 1024;
		mfxBS.Data = new mfxU8[mfxBS.MaxLength];
		MSDK_CHECK_POINTER(mfxBS.Data, MFX_ERR_MEMORY_ALLOC);

		// Read a chunk of data from stream file into bit stream buffer
		// - Parse bit stream, searching for header and fill video parameters structure
		// - Abort if bit stream header is not found in the first bit stream buffer chunk
		sts = ReadBitStreamData(&mfxBS, fSource);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		sts = mfxDEC.DecodeHeader(&mfxBS, &mfxVideoParams);
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// Initialize VPP parameters
		// - For simplistic memory management, system memory surfaces are used to store the raw frames
		//   (Note that when using HW acceleration D3D surfaces are prefered, for better performance)
		mfxVideoParam VPPParams;
		memset(&VPPParams, 0, sizeof(VPPParams));
		// Input data
		VPPParams.vpp.In.FourCC = MFX_FOURCC_NV12;
		VPPParams.vpp.In.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		VPPParams.vpp.In.CropX = 0;
		VPPParams.vpp.In.CropY = 0;
		VPPParams.vpp.In.CropW = mfxVideoParams.mfx.FrameInfo.CropW;
		VPPParams.vpp.In.CropH = mfxVideoParams.mfx.FrameInfo.CropH;
		VPPParams.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		VPPParams.vpp.In.FrameRateExtN = 30;
		VPPParams.vpp.In.FrameRateExtD = 1;
		// width must be a multiple of 16
		// height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
		VPPParams.vpp.In.Width = MSDK_ALIGN16(VPPParams.vpp.In.CropW);
		VPPParams.vpp.In.Height =
			(MFX_PICSTRUCT_PROGRESSIVE == VPPParams.vpp.In.PicStruct) ?
			MSDK_ALIGN16(VPPParams.vpp.In.CropH) :
			MSDK_ALIGN32(VPPParams.vpp.In.CropH);
		// Output data
		VPPParams.vpp.Out.FourCC = MFX_FOURCC_RGB4;
		VPPParams.vpp.Out.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
		VPPParams.vpp.Out.CropX = 0;
		VPPParams.vpp.Out.CropY = 0;
		VPPParams.vpp.Out.CropW = VPPParams.vpp.In.CropW / 2;   // Resize to half size resolution
		VPPParams.vpp.Out.CropH = VPPParams.vpp.In.CropH / 2;
		VPPParams.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
		VPPParams.vpp.Out.FrameRateExtN = 30;
		VPPParams.vpp.Out.FrameRateExtD = 1;
		// width must be a multiple of 16
		// height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
		VPPParams.vpp.Out.Width = MSDK_ALIGN16(VPPParams.vpp.Out.CropW);
		VPPParams.vpp.Out.Height =
			(MFX_PICSTRUCT_PROGRESSIVE == VPPParams.vpp.Out.PicStruct) ?
			MSDK_ALIGN16(VPPParams.vpp.Out.CropH) :
			MSDK_ALIGN32(VPPParams.vpp.Out.CropH);

		VPPParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;

		// Query number of required surfaces for decoder
		mfxFrameAllocRequest DecRequest;
		memset(&DecRequest, 0, sizeof(DecRequest));
		sts = mfxDEC.QueryIOSurf(&mfxVideoParams, &DecRequest);
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		mfxFrameAllocResponse DecResponse;
		sts = mfxAllocator.Alloc(mfxAllocator.pthis, &DecRequest, &DecResponse);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		// Query number of required surfaces for VPP
		mfxFrameAllocRequest VPPRequest[2];     // [0] - in, [1] - out
		memset(&VPPRequest, 0, sizeof(mfxFrameAllocRequest) * 2);
		sts = mfxVPP.QueryIOSurf(&VPPParams, VPPRequest);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		mfxFrameAllocResponse mfxResponseIn;
		mfxFrameAllocResponse mfxResponseOut;
		sts = mfxAllocator.Alloc(mfxAllocator.pthis, &VPPRequest[0], &mfxResponseIn);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		sts = mfxAllocator.Alloc(mfxAllocator.pthis, &VPPRequest[1], &mfxResponseOut);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// Determine the required number of surfaces for decoder output (VPP input) and for VPP output
		mfxU16 nSurfNumDecVPP = DecResponse.NumFrameActual + mfxResponseIn.NumFrameActual;
		mfxU16 nSurfNumVPPOut = mfxResponseOut.NumFrameActual;

		mfxFrameSurface1** pmfxSurfaces = new mfxFrameSurface1 *[nSurfNumDecVPP];
		MSDK_CHECK_POINTER(pmfxSurfaces, MFX_ERR_MEMORY_ALLOC);
		for (int i = 0; i < nSurfNumDecVPP; i++)
		{
			pmfxSurfaces[i] = new mfxFrameSurface1;
			memset(pmfxSurfaces[i], 0, sizeof(mfxFrameSurface1));

			if (i < DecResponse.NumFrameActual)
			{
				memcpy(&(pmfxSurfaces[i]->Info), &(mfxVideoParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
				pmfxSurfaces[i]->Data.MemId = DecResponse.mids[i];
			}
			else
			{
				memcpy(&(pmfxSurfaces[i]->Info), &(VPPParams.mfx.FrameInfo), sizeof(mfxFrameInfo));
				pmfxSurfaces[i]->Data.MemId = mfxResponseIn.mids[i - DecResponse.NumFrameActual];
			}
		}

		mfxFrameSurface1** pmfxSurfaces2 = new mfxFrameSurface1 *[nSurfNumVPPOut];
		MSDK_CHECK_POINTER(pmfxSurfaces2, MFX_ERR_MEMORY_ALLOC);
		for (int i = 0; i < nSurfNumVPPOut; i++)
		{
			pmfxSurfaces2[i] = new mfxFrameSurface1;
			memset(pmfxSurfaces2[i], 0, sizeof(mfxFrameSurface1));
			memcpy(&(pmfxSurfaces2[i]->Info), &(VPPParams.vpp.Out), sizeof(mfxFrameInfo));
			pmfxSurfaces2[i]->Data.MemId = mfxResponseOut.mids[i];
		}
		// Initialize the Media SDK decoder
		sts = mfxDEC.Init(&mfxVideoParams);
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// Initialize Media SDK VPP
		sts = mfxVPP.Init(&VPPParams);
		MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		// ===============================================================
		// Start decoding the frames from the stream
		//
		mfxTime tStart, tEnd;
		mfxGetTime(&tStart);

		mfxSyncPoint syncpD;
		mfxSyncPoint syncpV;
		mfxFrameSurface1* pmfxOutSurface = NULL;
		int nIndex = 0;
		int nIndex2 = 0;
		mfxU32 nFrame = 0;

		//
		// Stage 1: Main decoding loop
		//
		while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_DATA == sts || MFX_ERR_MORE_SURFACE == sts)
		{
			if (MFX_WRN_DEVICE_BUSY == sts)
				MSDK_SLEEP(1);  // Wait if device is busy, then repeat the same call to DecodeFrameAsync

			if (MFX_ERR_MORE_DATA == sts)
			{
				sts = ReadBitStreamData(&mfxBS, fSource);       // Read more data into input bit stream
				MSDK_BREAK_ON_ERROR(sts);
			}

			if (MFX_ERR_MORE_SURFACE == sts || MFX_ERR_NONE == sts)
			{
				nIndex = GetFreeSurfaceIndex(pmfxSurfaces, nSurfNumDecVPP);     // Find free frame surface
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nIndex, MFX_ERR_MEMORY_ALLOC);
			}
			// Decode a frame asychronously (returns immediately)
			sts = mfxDEC.DecodeFrameAsync(&mfxBS, pmfxSurfaces[nIndex], &pmfxOutSurface, &syncpD);

			// Ignore warnings if output is available,
			// if no output and no action required just repeat the DecodeFrameAsync call
			if (MFX_ERR_NONE < sts && syncpD)
				sts = MFX_ERR_NONE;

			if (MFX_ERR_NONE == sts)
			{
				nIndex2 = GetFreeSurfaceIndex(pmfxSurfaces2, nSurfNumVPPOut);   // Find free frame surface
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nIndex2, MFX_ERR_MEMORY_ALLOC);

				for (;;)
				{
					// Process a frame asychronously (returns immediately)
					sts = mfxVPP.RunFrameVPPAsync(pmfxOutSurface, pmfxSurfaces2[nIndex2], NULL, &syncpV);

					if (MFX_ERR_NONE < sts && !syncpV)
					{    // repeat the call if warning and no output
						if (MFX_WRN_DEVICE_BUSY == sts)
							MSDK_SLEEP(1);  // wait if device is busy
					}
					else if (MFX_ERR_NONE < sts && syncpV)
					{
						sts = MFX_ERR_NONE;     // ignore warnings if output is available
						break;
					}
					else
						break;  // not a warning
				}

				// VPP needs more data, let decoder decode another frame as input
				if (MFX_ERR_MORE_DATA == sts)
				{
					continue;
				}
				else if (MFX_ERR_MORE_SURFACE == sts)
				{
					// Not relevant for the illustrated workload! Therefore not handled.
					// Relevant for cases when VPP produces more frames at output than consumes at input. E.g. framerate conversion 30 fps -> 60 fps
					break;
				}
				else
					MSDK_BREAK_ON_ERROR(sts);
			}

			if (MFX_ERR_NONE == sts)
				sts = session.SyncOperation(syncpV, 60000);     // Synchronize. Wait until decoded frame is ready

			if (MFX_ERR_NONE == sts)
			{
				++nFrame;
				mfxFrameSurface1* ps = pmfxSurfaces2[nIndex2];
				sts = mfxAllocator.Lock(mfxAllocator.pthis, ps->Data.MemId, &(ps->Data));
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
				m_video2D.Copy(m_graphics.GetDX11(), NULL, pmfxSurfaces2[nIndex2]->Data.B, pmfxSurfaces2[nIndex2]->Info.CropH * pmfxSurfaces2[nIndex2]->Data.Pitch);
				sts = mfxAllocator.Unlock(mfxAllocator.pthis, ps->Data.MemId, &(ps->Data));
				MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

				m_graphics.GetDX11().SetRenderTexture2D(NULL);
				m_graphics.GetDX11().GetRender2D()->BeginDraw();
				m_graphics.DrawImage(&m_video2D, 1.0F, 1.0F);
				m_graphics.GetDX11().GetRender2D()->EndDraw();
				m_graphics.Present();

				Sleep(15);
			}
		}

		// MFX_ERR_MORE_DATA means that file has ended, need to go to buffering loop, exit in case of other errors
		MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		//
		// Stage 2: Retrieve the buffered decoded frames
		//
		while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_SURFACE == sts)
		{
			if (MFX_WRN_DEVICE_BUSY == sts)
				MSDK_SLEEP(1);  // Wait if device is busy, then repeat the same call to DecodeFrameAsync

			nIndex = GetFreeSurfaceIndex(pmfxSurfaces, nSurfNumDecVPP);     // Find free frame surface
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nIndex, MFX_ERR_MEMORY_ALLOC);

			// Decode a frame asychronously (returns immediately)
			sts = mfxDEC.DecodeFrameAsync(NULL, pmfxSurfaces[nIndex], &pmfxOutSurface, &syncpD);

			// Ignore warnings if output is available,
			// if no output and no action required just repeat the DecodeFrameAsync call
			if (MFX_ERR_NONE < sts && syncpD)
				sts = MFX_ERR_NONE;

			if (MFX_ERR_NONE == sts)
			{
				nIndex2 = GetFreeSurfaceIndex(pmfxSurfaces2, nSurfNumVPPOut);   // Find free frame surface
				MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nIndex2, MFX_ERR_MEMORY_ALLOC);

				for (;;)
				{
					// Process a frame asychronously (returns immediately)
					sts = mfxVPP.RunFrameVPPAsync(pmfxOutSurface, pmfxSurfaces2[nIndex2], NULL, &syncpV);

					if (MFX_ERR_NONE < sts && !syncpV)
					{    // repeat the call if warning and no output
						if (MFX_WRN_DEVICE_BUSY == sts)
							MSDK_SLEEP(1);  // wait if device is busy
					}
					else if (MFX_ERR_NONE < sts && syncpV)
					{
						sts = MFX_ERR_NONE;     // ignore warnings if output is available
						break;
					}
					else
						break;  // not a warning
				}

				// VPP needs more data, let decoder decode another frame as input
				if (MFX_ERR_MORE_DATA == sts)
				{
					continue;
				}
				else if (MFX_ERR_MORE_SURFACE == sts)
				{
					// Not relevant for the illustrated workload! Therefore not handled.
					// Relevant for cases when VPP produces more frames at output than consumes at input. E.g. framerate conversion 30 fps -> 60 fps
					break;
				}
				else
					MSDK_BREAK_ON_ERROR(sts);
			}

			if (MFX_ERR_NONE == sts)
				sts = session.SyncOperation(syncpV, 60000);     // Synchronize. Waits until decoded frame is ready

			if (MFX_ERR_NONE == sts)
			{
				++nFrame;
				//if (bEnableOutput)
				{
					/*sts = WriteRawFrame(pmfxSurfaces2[nIndex2], fSink);
					MSDK_BREAK_ON_ERROR(sts);

					printf("Frame number: %d\r", nFrame);*/
				}
			}
		}

		// MFX_ERR_MORE_DATA means that decoder is done with buffered frames, need to go to VPP buffering loop, exit in case of other errors
		MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		//
		// Stage 3: Retrieve the buffered VPP frames
		//
		while (MFX_ERR_NONE <= sts) {
			nIndex2 = GetFreeSurfaceIndex(pmfxSurfaces2, nSurfNumVPPOut);   // Find free frame surface
			MSDK_CHECK_ERROR(MFX_ERR_NOT_FOUND, nIndex2, MFX_ERR_MEMORY_ALLOC);

			// Process a frame asychronously (returns immediately)
			sts = mfxVPP.RunFrameVPPAsync(NULL, pmfxSurfaces2[nIndex2], NULL, &syncpV);
			MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_SURFACE);
			MSDK_BREAK_ON_ERROR(sts);

			sts = session.SyncOperation(syncpV, 60000);     // Synchronize. Wait until frame processing is ready
			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

			++nFrame;
			//if (bEnableOutput)
			{
				/*sts = WriteRawFrame(pmfxSurfaces2[nIndex2], fSink);
				MSDK_BREAK_ON_ERROR(sts);

				printf("Frame number: %d\r", nFrame);*/
			}
		}

		// MFX_ERR_MORE_DATA indicates that all buffers has been fetched, exit in case of other errors
		MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	}
}