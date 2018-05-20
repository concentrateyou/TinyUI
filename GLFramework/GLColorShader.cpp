#include "stdafx.h"
#include "GLColorShader.h"

namespace GLFramework
{
	GLColorShader::GLColorShader()
		:m_vertexShader(NULL),
		m_fragmentShader(NULL),
		m_shaderID(NULL)
	{
	}

	GLColorShader::~GLColorShader()
	{
		GL::GetAPI().glDetachShader(m_shaderID, m_vertexShader);
		GL::GetAPI().glDetachShader(m_shaderID, m_fragmentShader);
		GL::GetAPI().glDeleteShader(m_vertexShader);
		GL::GetAPI().glDeleteShader(m_fragmentShader);
		GL::GetAPI().glDeleteProgram(m_shaderID);
	}

	BOOL GLColorShader::Initialize(GL& gl, const CHAR* vsFile, const CHAR* psFile)
	{
		TinyFile sFile;
		if (!sFile.Open((LPCTSTR)vsFile))
			return FALSE;
		UINT size = static_cast<UINT>(sFile.GetSize() + 1);
		TinyScopedArray<CHAR> bits(new CHAR[size]);
		if (!bits)
			return FALSE;
		UINT count = static_cast<UINT>(sFile.Read(bits, size - 1));
		ASSERT(count == (size - 1));
		sFile.Close();
		bits[count] = '\0';
		m_vertexShader = GL::GetAPI().glCreateShader(GL_VERTEX_SHADER);
		GL::GetAPI().glShaderSource(m_vertexShader, 1, (const CHAR**)&bits, NULL);
		GL::GetAPI().glCompileShader(m_vertexShader);
		INT status = 0;
		GL::GetAPI().glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
		if (status != 1)
		{
			INT logsize = 0;
			GL::GetAPI().glGetShaderiv(m_vertexShader, GL_INFO_LOG_LENGTH, &logsize);
			logsize += 1;
			TinyScopedArray<CHAR> log(new CHAR[logsize]);
			GL::GetAPI().glGetShaderInfoLog(m_vertexShader, logsize, NULL, log);
			LOG(ERROR) << "glCompileShader: " << vsFile << " Error:" << log;

		}
		//PS
		sFile.Close();
		if (!sFile.Open((LPCTSTR)psFile))
			return FALSE;
		size = static_cast<UINT>(sFile.GetSize() + 1);
		bits.Reset(new CHAR[size]);
		if (!bits)
			return FALSE;
		count = sFile.Read(bits, size - 1);
		ASSERT(count == (size - 1));
		sFile.Close();
		bits[count] = '\0';
		m_fragmentShader = GL::GetAPI().glCreateShader(GL_FRAGMENT_SHADER);
		GL::GetAPI().glShaderSource(m_fragmentShader, 1, (const CHAR**)&bits, NULL);
		GL::GetAPI().glCompileShader(m_fragmentShader);
		GL::GetAPI().glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &status);
		if (status != 1)
		{
			INT logsize = 0;
			GL::GetAPI().glGetShaderiv(m_fragmentShader, GL_INFO_LOG_LENGTH, &logsize);
			logsize += 1;
			TinyScopedArray<CHAR> log(new CHAR[logsize]);
			GL::GetAPI().glGetShaderInfoLog(m_fragmentShader, logsize, NULL, log);
			LOG(ERROR) << "glCompileShader: " << psFile << " Error:" << log;
			return FALSE;
		}
		m_shaderID = GL::GetAPI().glCreateProgram();
		GL::GetAPI().glAttachShader(m_shaderID, m_vertexShader);
		GL::GetAPI().glAttachShader(m_shaderID, m_fragmentShader);
		GL::GetAPI().glBindAttribLocation(m_shaderID, 0, "v_position");
		GL::GetAPI().glBindAttribLocation(m_shaderID, 1, "v_texCoord");
		GL::GetAPI().glBindAttribLocation(m_shaderID, 2, "v_color");
		GL::GetAPI().glLinkProgram(m_shaderID);
		GL::GetAPI().glGetProgramiv(m_shaderID, GL_LINK_STATUS, &status);
		if (status != 1)
		{
			INT logsize = 0;
			GL::GetAPI().glGetProgramiv(m_shaderID, GL_INFO_LOG_LENGTH, &logsize);
			logsize += 1;
			TinyScopedArray<CHAR> log(new CHAR[logsize]);
			GL::GetAPI().glGetProgramInfoLog(m_shaderID, logsize, NULL, log);
			LOG(ERROR) << "glLinkProgram Error:" << log;
			return FALSE;
		}
		GL::GetAPI().glUseProgram(m_shaderID);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
	}
	void GLColorShader::Render(GL& gl)
	{
		UNUSED(gl);
		GL::GetAPI().glUseProgram(m_shaderID);
	}
	BOOL GLColorShader::SetShaderParameters(GL& gl, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
	{
		FLOAT matrix[16];
		XMFLOAT4X4 view;
		ZeroMemory(matrix, 16 * sizeof(FLOAT));
		ZeroMemory(&view, sizeof(view));
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
		GLuint iRes = GL::GetAPI().glGetUniformLocation(m_shaderID, "worldMatrix");
		if (iRes == -1)
			return FALSE;
		GL::GetAPI().glUniformMatrix4fv(iRes, 1, FALSE, matrix);
		iRes = GL::GetAPI().glGetUniformLocation(m_shaderID, "viewMatrix");
		if (iRes == -1)
			return FALSE;
		ZeroMemory(matrix, 16 * sizeof(FLOAT));
		ZeroMemory(&view, sizeof(view));
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
		GL::GetAPI().glUniformMatrix4fv(iRes, 1, FALSE, matrix);
		iRes = GL::GetAPI().glGetUniformLocation(m_shaderID, "projectionMatrix");
		if (iRes == -1)
			return FALSE;
		ZeroMemory(matrix, 16 * sizeof(FLOAT));
		ZeroMemory(&view, sizeof(view));
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
		GL::GetAPI().glUniformMatrix4fv(iRes, 1, FALSE, matrix);
		iRes = GL::GetAPI().glGetUniformLocation(m_shaderID, "shaderTexture");
		if (iRes == -1)
			return FALSE;
		GL::GetAPI().glUniform1i(iRes, 0);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
	}
}