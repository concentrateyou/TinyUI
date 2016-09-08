#pragma once
#include "DXFramework.h"

enum EncoderType
{
	ENCODER_AUDIO,
	ENCODER_VIDEO
};

typedef struct tagEncoderPacket
{
	BYTE*		bits;
	INT			size;
	INT			dts;
	INT			pts;
	INT			duration;
	LONG		timestamp;
	EncoderType	type;
}EncoderPacket;

