#pragma once
#include "pch.h"
#include "interfaces.h"
#include "colorshader.h"

class OpenGLRenderer;

class Entity_PointCloud : public Entity
{
public:

public:
	Entity_PointCloud();
	Entity_PointCloud(const Entity_PointCloud& _other) = default;
	~Entity_PointCloud() = default;

	bool Initialize(OpenGLRenderer* _renderer, ColorShader::VertexType* _points, int _numPoints);

	void Update();
	void Shutdown();
	void Render(float* _viewMat);
	void Reset();

	void SetPosition(float _x, float _y, float _z);
	void SetRotation(float _x, float _y, float _z);

	void GetPosition(float* _result);
	void GetRotation(float* _result);

	void GetWorldMatrix(float* _result);
	inline int GetShaderID() { return ColorShader::SHADER_ID; }

private:
	bool _initializeBuffers(ColorShader::VertexType*, int);
	void _shutdownBuffers();
	void _renderBuffers();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_worldMatrix[16];
	int m_vertexCount;
	unsigned int m_vertexArrayId, m_vertexBufferId;

	OpenGLRenderer* m_Renderer;
};