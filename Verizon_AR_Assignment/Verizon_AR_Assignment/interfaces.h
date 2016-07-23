#pragma once
#include "pch.h"

class OpenGLRenderer;

class Scene
{
public:
	virtual void Shutdown() = 0;
	virtual bool Update() = 0;
	virtual bool Render() = 0;
	virtual void Action() = 0;
	virtual void Reset() = 0;
};

class Entity
{
public:
	virtual void Shutdown() = 0;
	virtual void Render(float*) = 0;
	virtual void Update() = 0;
	virtual void Reset() = 0;
	virtual int GetShaderID() = 0;
};

class Shader
{
public:
	virtual void Shutdown() = 0;
	virtual void SetShader() = 0;
};