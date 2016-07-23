#include "texture.h"
#include <stdio.h>

Texture::Texture()
{
	loaded = false;
}

void Texture::Set(OpenGLRenderer* _renderer, int _textureUnit)
{
	_renderer->glActiveTexture(GL_TEXTURE0 + _textureUnit);

	// Bind the texture as a 2D texture.
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

bool Texture::InitializeFromMemory(OpenGLRenderer* _renderer, char* _data, int _width, int _height, int _bpp, unsigned int _textureUnit, bool _wrap)
{
	int error, imageSize;
	unsigned int count;

	// Check that it is 32 bit and not 24 bit.
	if (_bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = _width * _height * 4;

	// Set the unique texture unit in which to store the data.
	_renderer->glActiveTexture(GL_TEXTURE0 + _textureUnit);

	// Generate an ID for the texture.
	glGenTextures(1, &m_textureID);

	// Bind the texture as a 2D texture.
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	// Load the image data into the texture unit.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _data);

	// Set the texture color to either wrap around or clamp to the edge.
	if (_wrap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	// Set the texture filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Generate mipmaps for the texture.
	_renderer->glGenerateMipmap(GL_TEXTURE_2D);

	// Set that the texture is loaded.
	loaded = true;

	return true;
}

void Texture::Shutdown()
{
	// If the texture was loaded then make sure to release it on shutdown.
	if (loaded)
	{
		glDeleteTextures(1, &m_textureID);
		loaded = false;
	}

	return;
}

bool Texture::UpdateTexture(OpenGLRenderer* _renderer, char* _data, int _width, int _height, int _textureUnit)
{
	_renderer->glActiveTexture(GL_TEXTURE0 + _textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _data);
	_renderer->glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}
