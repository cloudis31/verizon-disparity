#include "appcontext.h"
#include "window.h"
#include "ogl.h"
#include "scenes.h"

void AppContext::Run()
{
	int windowWidth = 1064;
	int windowHeight = 600;

	// create main window
	m_MainWindow = new Window(L"Verizon");
	if (!m_MainWindow->Initialize(windowWidth, windowHeight, DefaultWndProc, false))
	{
		MessageBox(NULL, L"Could not initialize the main window.", L"Error", MB_OK);
		return;
	}

	// create renderer
	m_Renderer = new OpenGLRenderer();

	// initialize opengl extentions. Need a temp window for this
	HWND tmphwnd = Window::CreateTempWindow();
	if(!m_Renderer->InitializeExtensionFunctions(tmphwnd))
	{
		MessageBox(NULL, L"Could not initialize the OpenGL extension functions.", L"Error", MB_OK);
		return;
	}
	Window::DestroyTempWindow();

	// initialize renderer
	if (!m_Renderer->Initialize(m_MainWindow->GetHWND(), windowWidth, windowHeight, 1.0f, 1000.0f, true))
	{
		MessageBox(NULL, L"Could not initialize the OpenGL Renderer.", L"Error", MB_OK);
		return;
	}

	// show the window now
	m_MainWindow->Show();

	// create first and second assignment scene and push it back
	Scene_Assignment1_2* scene12 = new Scene_Assignment1_2();
	scene12->Initialize(m_Renderer, m_MainWindow->GetHWND());
	m_Scenes.push_back(scene12);

	// create third assignment scene and push it back
	Scene_Assignment3* scene3 = new Scene_Assignment3();
	scene3->Initialize(m_Renderer, m_MainWindow->GetHWND());
	m_Scenes.push_back(scene3);

	m_CurrScene = 0;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	bool running = true;
	while (running)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			running = false;
		}
		else
		{
			// update then render the scene
			m_Scenes[m_CurrScene]->Update();
			m_Scenes[m_CurrScene]->Render();
		}
	}
}


LRESULT AppContext::HandleMessage(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch (_msg)
	{
		// check for keyboard input
		case WM_KEYDOWN:
		{
			switch (_wparam)
			{
				// escape key was pressed
				case VK_ESCAPE:
					PostQuitMessage(0);
					return 0;
				case VK_SPACE:
					m_Scenes[m_CurrScene]->Action();
					return 0;
				case VK_RIGHT:
					m_CurrScene++;
					if (m_CurrScene >= m_Scenes.size())
						m_CurrScene = 0;
					return 0;
			}
		}
		case WM_CHAR:
		{
			switch (_wparam)
			{
			case 'r':
				m_Scenes[m_CurrScene]->Reset();
				break;
			}
			return 0;
		}
	}
	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);

}

LRESULT CALLBACK DefaultWndProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch (_msg)
	{
		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		} 

		// All other messages pass to the input handler class.
		default:
		{
			return AppContext::Instance()->HandleMessage(_hwnd, _msg, _wparam, _lparam);
		}
	}
}