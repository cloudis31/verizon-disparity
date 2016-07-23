#pragma once
using namespace std;

#include "ogl.h"
#include "interfaces.h"

class OpenGLRenderer;

class TextureShader : public Shader
{
public:
	static const int SHADER_ID = 1;

	struct VertexType
	{
		float x, y, z;
		float tu, tv;
	};

public:
	TextureShader() = default;
	TextureShader(const TextureShader& _other) = default;
	~TextureShader() = default;

	bool Initialize(OpenGLRenderer* _renderer, HWND _hwnd);
	void Shutdown();
	void SetShader();

	bool SetShaderParameters(float* _worldMatrix, float* _viewMatrix, float* _projectionMatrix, int _textureUnit);

private:
	bool _initializeShader(char* _vsFilename, char* _psFilename, HWND _hwnd);
	char* _loadShaderSourceFile(char* _filename);
	void _outputShaderErrorMessage(HWND _hwnd, unsigned int _shaderId, char* _shaderFilename);
	void _outputLinkerErrorMessage(HWND _hwnd, unsigned int _programId);
	void _shutdownShader();

private:
	unsigned int m_vertexShader;
	unsigned int m_pixelShader;
	unsigned int m_shaderProgram;

	OpenGLRenderer* m_Renderer;
};