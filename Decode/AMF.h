#pragma once
#include "Utility.h"
using namespace TinyUI;

namespace Decode
{

#define AMF_TYPE_NUMBER             ((BYTE)0x00)
#define AMF_TYPE_BOOLEAN	        ((BYTE)0x01)
#define AMF_TYPE_STRING	            ((BYTE)0x02)
#define AMF_TYPE_OBJECT	            ((BYTE)0x03)
#define AMF_TYPE_NULL               ((BYTE)0x05)
#define AMF_TYPE_UNDEFINED	        ((BYTE)0x06)
#define AMF_TYPE_ASSOCIATIVE_ARRAY	((BYTE)0x08)
#define AMF_TYPE_END                ((BYTE)0x09)
#define AMF_TYPE_ARRAY	            ((BYTE)0x0A)
#define AMF_TYPE_DATE	            ((BYTE)0x0B)
#define AMF_TYPE_XML	            ((BYTE)0x0F)
#define AMF_TYPE_CLASS	            ((BYTE)0x10)


}



