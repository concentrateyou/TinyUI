#include "stdafx.h"
#include "GLTextureShader.h"

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
		glClearErrors();
		LONGLONG size = sFile.GetSize() + 1;
		TinyScopedArray<CHAR> bits(new CHAR[size]);
		if (!bits)
			return FALSE;
		DWORD dwCount = sFile.Read(bits, size - 1);
		ASSERT(dwCount == (size - 1));
		bits[dwCount] = '\0';
		m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
		if (!GLSuccess() || !m_vertexShader)
			return FALSE;
		glShaderSource(m_vertexShader, 1, (const GLchar**)&bits, NULL);
		if (!GLSuccess())
			return FALSE;
		if (!sFile.Open((LPCTSTR)psFile))
			return FALSE;
		size = sFile.GetSize() + 1;
		bits.Reset(new CHAR[size]);
		if (!bits)
			return FALSE;
		dwCount = sFile.Read(bits, size - 1);
		ASSERT(dwCount == (size - 1));
		bits[dwCount] = '\0';
		m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if (!GLSuccess() || !m_fragmentShader)
			return FALSE;
		glShaderSource(m_fragmentShader, 1, (const GLchar**)&bits, NULL);
		if (!GLSuccess())
			return FALSE;
		glCompileShader(m_vertexShader);
		if (!GLSuccess())
			return FALSE;
		glCompileShader(m_fragmentShader);
		if (!GLSuccess())
			return FALSE;
		INT status = 0;
		glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
		if (status != 1)
		{
			INT logsize = 0;
			glGetShaderiv(m_vertexShader, GL_INFO_LOG_LENGTH, &logsize);
			logsize += 1;
			TinyScopedArray<CHAR> log(new CHAR[logsize]);
			glGetShaderInfoLog(m_vertexShader, logsize, NULL, log);
			LOG(ERROR) << "glCompileShader: " << vsFile << " Error:" << log;

		}
		glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &status);
		if (status != 1)
		{
			INT logsize = 0;
			glGetShaderiv(m_fragmentShader, GL_INFO_LOG_LENGTH, &logsize);
			logsize += 1;
			TinyScopedArray<CHAR> log(new CHAR[logsize]);
			glGetShaderInfoLog(m_fragmentShader, logsize, NULL, log);
			LOG(ERROR) << "glCompileShader: " << psFile << " Error:" << log;
			return FALSE;
		}
		m_shaderID = glCreateProgram();
		if (!GLSuccess() || !m_shaderID)
			return FALSE;
		glAttachShader(m_shaderID, m_vertexShader);
		if (!GLSuccess())
			return FALSE;
		glAttachShader(m_shaderID, m_fragmentShader);
		if (!GLSuccess())
			return FALSE;
		glBindAttribLocation(m_shaderID, 0, "v_position");
		if (!GLSuccess())
			return FALSE;
		glBindAttribLocation(m_shaderID, 1, "v_texCoord");
		if (!GLSuccess())
			return FALSE;
		glBindAttribLocation(m_shaderID, 2, "v_color");
		if (!GLSuccess())
			return FALSE;
		glLinkProgram(m_shaderID);
		if (!GLSuccess())
			return FALSE;
		glGetProgramiv(m_shaderID, GL_LINK_STATUS, &status);
		if (status != 1)
		{
			INT logsize = 0;
			glGetProgramiv(m_fragmentShader, GL_INFO_LOG_LENGTH, &logsize);
			logsize += 1;
			TinyScopedArray<CHAR> log(new CHAR[logsize]);
			glGetProgramInfoLog(m_shaderID, logsize, NULL, log);
			LOG(ERROR) << "glLinkProgram Error:" << log;
			return FALSE;
		}
		glUseProgram(m_shaderID);
		if (!GLSuccess())
			return FALSE;
		glClearErrors();
		return TRUE;
	}

	BOOL GLTextureShader::SetShaderParameters(GL& gl, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const GLint& textureID)
	{
		GLuint lRes = glGetUniformLocation(m_shaderID, "worldMatrix");
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
		glUniformMatrix4fv(lRes, 1, FALSE, matrix);
		lRes = glGetUniformLocation(m_shaderID, "viewMatrix");
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
		glUniformMatrix4fv(lRes, 1, FALSE, matrix);
		lRes = glGetUniformLocation(m_shaderID, "projectionMatrix");
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
		glUniformMatrix4fv(lRes, 1, FALSE, matrix);
		lRes = glGetUniformLocation(m_shaderID, "shaderTexture");
		if (lRes == -1)
			return FALSE;
		glUniform1i(lRes, textureID);
		return TRUE;
	}

	void GLTextureShader::Render(GL& gl)
	{

	}
}