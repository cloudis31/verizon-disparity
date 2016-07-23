#pragma once
#include "pch.h"
#include "interfaces.h"
#include "textureshader.h"
#include <opencv2\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

class Texture;
class OpenGLRenderer;

class Entity_FullscreenQuad : public Entity
{
public:
	Entity_FullscreenQuad();
	Entity_FullscreenQuad(const Entity_FullscreenQuad& _other) = default;
	~Entity_FullscreenQuad() = default;

	bool Initialize(OpenGLRenderer* _renderer);

	bool LoadTextureFromMat(cv::Mat _image, unsigned int _textureUnit, bool _wrap);
	bool LoadTextureFromMemory(char* _data, int _width, int _height, unsigned int _textureUnit, bool _wrap);
	bool UpdateTexture(char* _data, int _width, int _height, int _textureUnit);

	void Update();
	void Shutdown();
	void Render(float* _viewMat);
	void Reset();

	void SetPosition(float _x, float _y, float _z);
	void SetRotation(float _x, float _y, float _z);

	void GetWorldMatrix(float* _result);
	inline int GetShaderID() { return TextureShader::SHADER_ID; }
	
private:
	bool _initializeBuffers();
	void _shutdownBuffers();
	void _renderBuffers();
	void _releaseTexture();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_worldMatrix[16];
	int m_vertexCount, m_indexCount;
	unsigned int m_vertexArrayId, m_vertexBufferId, m_indexBufferId;

	Texture* m_Texture;
	OpenGLRenderer* m_Renderer;
};