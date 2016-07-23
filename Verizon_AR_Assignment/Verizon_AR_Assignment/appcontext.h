#pragma once

#include "pch.h"

class OpenGLRenderer;
class Window;
class Scene;

class AppContext
{
	AppContext() = default;
	AppContext(const AppContext& _other) = delete;

public:
	~AppContext() = default;

	static AppContext* Instance()
	{
		static AppContext* instance;
		if (!instance)
		{
			instance = new AppContext();
		}
		return instance;
	}

	void Run();

	LRESULT HandleMessage(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);

private:
	std::vector<Scene*> m_Scenes;
	int m_CurrScene = 0;

	OpenGLRenderer* m_Renderer;
	Window* m_MainWindow;
};

LRESULT CALLBACK DefaultWndProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);