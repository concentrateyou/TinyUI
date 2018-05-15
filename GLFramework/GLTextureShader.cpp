#include "stdafx.h"
#include "GLTextureShader.h"
#include "IO/TinyIO.h"
using namespace TinyUI::IO;

namespace GLFramework
{
	GLTextureShader::GLTextureShader()
		:m_vertexShader(0),
		m_fragmentShader(0),
		m_shaderID(0)
	{
	}

	GLTextureShader::~GLTextureShader()
	{
	}

	BOOL GLTextureShader::Initialize(GL& gl, const CHAR* vsFile, const CHAR* psFile)
	{
		TinyFile sFile;
		if (!sFile.Open((LPCTSTR)vsFile))
			return FALSE;
		LONGLONG size = sFile.GetSize() + 1;
		TinyScopedArray<CHAR> bits(new CHAR[size]);
		if (!bits)
			return FALSE;
		DWORD dwCount = sFile.Read(bits, size - 1);
		ASSERT(dwCount != (size - 1));
		bits[dwCount] = '\0';
		m_vertexShader = gl.glCreateShader(GL_VERTEX_SHADER);
		gl.glShaderSource(m_vertexShader, 1, (const GLchar**)&bits, NULL);
		if (!sFile.Open((LPCTSTR)psFile))
			return FALSE;
		size = sFile.GetSize() + 1;
		bits.Reset(new CHAR[size]);
		if (!bits)
			return FALSE;
		dwCount = sFile.Read(bits, size - 1);
		ASSERT(dwCount != (size - 1));
		bits[dwCount] = '\0';
		m_fragmentShader = gl.glCreateShader(GL_FRAGMENT_SHADER);
		gl.glShaderSource(m_fragmentShader, 1, (const GLchar**)&bits, NULL);
		gl.glCompileShader(m_vertexShader);
		gl.glCompileShader(m_fragmentShader);
		INT status = 0;
		gl.glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
		if (status != 1)
		{
			INT logsize = 0;
			gl.glGetShaderiv(m_vertexShader, GL_INFO_LOG_LENGTH, &logsize);
			logsize += 1;
			TinyScopedArray<CHAR> log(new CHAR[logsize]);
			gl.glGetShaderInfoLog(m_vertexShader, logsize, NULL, log);
			LOG(ERROR) << "glCompileShader: " << vsFile << " Error:" << log;

		}
		gl.glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &status);
		if (status != 1)
		{
			INT logsize = 0;
			gl.glGetShaderiv(m_fragmentShader, GL_INFO_LOG_LENGTH, &logsize);
			logsize += 1;
			TinyScopedArray<CHAR> log(new CHAR[logsize]);
			gl.glGetShaderInfoLog(m_fragmentShader, logsize, NULL, log);
			LOG(ERROR) << "glCompileShader: " << psFile << " Error:" << log;
			return FALSE;
		}
		m_shaderID = gl.glCreateProgram();
		ASSERT(m_shaderID);
		gl.glAttachShader(m_shaderID, m_vertexShader);
		gl.glAttachShader(m_shaderID, m_fragmentShader);
		gl.glBindAttribLocation(m_shaderID, 0, "inputPosition");
		gl.glBindAttribLocation(m_shaderID, 1, "inputTexCoord");
		gl.glBindAttribLocation(m_shaderID, 2, "inputColor");
		gl.glLinkProgram(m_shaderID);
		gl.glGetProgramiv(m_shaderID, GL_LINK_STATUS, &status);
		if (status != 1)
		{
			INT logsize = 0;
			gl.glGetProgramiv(m_fragmentShader, GL_INFO_LOG_LENGTH, &logsize);
			logsize += 1;
			TinyScopedArray<CHAR> log(new CHAR[logsize]);
			gl.glGetProgramInfoLog(m_shaderID, logsize, NULL, log);
			LOG(ERROR) << "glLinkProgram Error:" << log;
			return FALSE;
		}
		gl.glUseProgram(m_shaderID);
		return TRUE;
	}

	BOOL GLTextureShader::SetShaderParameters(GL& gl, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
	{
		GLuint lRes = gl.glGetUniformLocation(m_shaderID, "worldMatrix");
		if (lRes == -1)
			return FALSE;
		FLOAT matrix[16];
		XMFLOAT4X4 view;
		XMStoreFloat4x4(&view, worldMatrix);
		matrix[0] = view._11;
		matrix[1] = view._12;
		matrix[2] = view._13;
		matrix[3] = view._14;
		matrix[4] = view._21;
		matrix[5] = view._22;
		matrix[6] = view._23;
		matrix[7] = view._24;
		matrix[8] = view._31;
		matrix[9] = view._32;
		matrix[10] = view._33;
		matrix[11] = view._34;
		matrix[12] = view._41;
		matrix[13] = view._42;
		matrix[14] = view._43;
		matrix[15] = view._44;
		gl.glUniformMatrix4fv(lRes, 1, FALSE, matrix);
		lRes = gl.glGetUniformLocation(m_shaderID, "viewMatrix");
		if (lRes == -1)
			return FALSE;
		XMStoreFloat4x4(&view, viewMatrix);
		matrix[0] = view._11;
		matrix[1] = view._12;
		matrix[2] = view._13;
		matrix[3] = view._14;
		matrix[4] = view._21;
		matrix[5] = view._22;
		matrix[6] = view._23;
		matrix[7] = view._24;
		matrix[8] = view._31;
		matrix[9] = view._32;
		matrix[10] = view._33;
		matrix[11] = view._34;
		matrix[12] = view._41;
		matrix[13] = view._42;
		matrix[14] = view._43;
		matrix[15] = view._44;
		gl.glUniformMatrix4fv(lRes, 1, FALSE, matrix);
		lRes = gl.glGetUniformLocation(m_shaderID, "projectionMatrix");
		if (lRes == -1)
			return FALSE;
		XMStoreFloat4x4(&view, projectionMatrix);
		matrix[0] = view._11;
		matrix[1] = view._12;
		matrix[2] = view._13;
		matrix[3] = view._14;
		matrix[4] = view._21;
		matrix[5] = view._22;
		matrix[6] = view._23;
		matrix[7] = view._24;
		matrix[8] = view._31;
		matrix[9] = view._32;
		matrix[10] = view._33;
		matrix[11] = view._34;
		matrix[12] = view._41;
		matrix[13] = view._42;
		matrix[14] = view._43;
		matrix[15] = view._44;
		gl.glUniformMatrix4fv(lRes, 1, FALSE, matrix);
		return TRUE;
	}

	void GLTextureShader::Render(GL& gl)
	{

	}
}