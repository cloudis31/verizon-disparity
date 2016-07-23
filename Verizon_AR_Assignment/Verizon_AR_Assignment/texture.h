#pragma once
#include "ogl.h"

class Texture
{
public:
	Texture();
	Texture(const Texture& _other) = default;
	~Texture() = default;
	bool InitializeFromMemory(OpenGLRenderer* _renderer, char* _data, int _width, int _height, int _bpp, unsigned int _textureUnit, bool _wrap);
	bool UpdateTexture(OpenGLRenderer* _renderer, char* _data, int _width, int _height, int _textureUnit);
	void Shutdown();

	void Set(OpenGLRenderer* _renderer, int _textureUnit);

private:
	bool loaded;
	unsigned int m_textureID;
};