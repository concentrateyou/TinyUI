#pragma once
#include "GL.h"
#include "GLShader.h"

namespace GLFramework
{
	class GLColorShader : public GLShader
	{
		DISALLOW_COPY_AND_ASSIGN(GLColorShader)
	public:
		GLColorShader();
		virtual ~GLColorShader();
	public:
		BOOL Initialize(GL& gl, const CHAR* vsFile, const CHAR* psFile) OVERRIDE;
		void Render(GL& gl) OVERRIDE;
	public:
		BOOL SetShaderParameters(GL& gl, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix);
	private:
		GLuint m_vertexShader;
		GLuint m_fragmentShader;
		GLuint m_shaderID;
	};
}