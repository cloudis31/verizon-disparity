#pragma once
#include "interfaces.h"
#include "entities.h"
#include <opencv2\core.hpp>

class OpenGLRenderer;
class Camera;
class Entity;
class TextureShader;

class Scene_Assignment1_2 : public Scene
{
public:
	Scene_Assignment1_2();
	Scene_Assignment1_2(const Scene_Assignment1_2& _other) = default;
	~Scene_Assignment1_2() = default;

	bool Initialize(OpenGLRenderer* _renderer, HWND _hwnd);
	void Shutdown();
	bool Update();
	bool Render();
	void Action();
	void Reset();

private:
	bool _createFullScreenQuad(cv::Mat _image, HWND _hwnd);
	bool _createPointCloud(ColorShader::VertexType* _vertices, int _numVerts, HWND _hwnd);

private:
	OpenGLRenderer* m_Renderer;
	Camera* m_Camera;
	std::vector<Entity*> m_Entities;

	char* m_TextureData;
	int m_CurrTexturePos;

	int m_VisibleEntity;
};