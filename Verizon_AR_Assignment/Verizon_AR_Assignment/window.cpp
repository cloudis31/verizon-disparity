#include "window.h"

HWND Window::m_TmpHwnd = NULL;
HINSTANCE Window::m_TmpHInstance = NULL;
LPCWSTR Window::m_TmpWinName = L"tmpwin";

HWND Window::CreateTempWindow()
{
	if (!m_TmpHwnd)
	{
		m_TmpHInstance = GetModuleHandle(NULL);

		WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = [](HWND h, UINT m, WPARAM w, LPARAM l)->LRESULT
		{
			if (m == WM_CLOSE)
				PostQuitMessage(0);
			else
				return DefWindowProc(h, m, w, l);
			return 0;
		};
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hInstance = m_TmpHInstance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = m_TmpWinName;
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		// Register the window class.
		RegisterClassEx(&wc);

		m_TmpHwnd = CreateWindowEx(WS_EX_APPWINDOW, m_TmpWinName, m_TmpWinName, WS_POPUP,
			0, 0, 640, 480, NULL, NULL, m_TmpHInstance, NULL);

		// don't show this window
		ShowWindow(m_TmpHwnd, SW_HIDE);
	}
	return m_TmpHwnd;

}
void Window::DestroyTempWindow()
{
	DestroyWindow(m_TmpHwnd);
	m_TmpHwnd = NULL;
	UnregisterClass(m_TmpWinName, m_TmpHInstance);
	m_TmpHInstance = NULL;
}

bool Window::Initialize(int _width, int _height, WNDPROC _wndproc, bool _fullscreen)
{
	m_Fullscreen = _fullscreen;

	DEVMODE dmScreenSettings;
	int posX, posY;
	bool result;

	// Get the instance of this application.
	m_HandleInstance = GetModuleHandle(NULL);

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = _wndproc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = m_HandleInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_WindowName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Register the window class.
	RegisterClassEx(&wc);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (m_Fullscreen)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)_width;
		dmScreenSettings.dmPelsHeight = (unsigned long)_height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - _width) * 0.5f;
		posY = (GetSystemMetrics(SM_CYSCREEN) - _height) * 0.5f;
		//posX = 0;
		//posY = 0;
	}

	// Create the window with the screen settings and get the handle to it.
	m_WindowHandle = CreateWindowEx(WS_EX_APPWINDOW, m_WindowName, m_WindowName, WS_POPUP,
		posX, posY, _width, _height, NULL, NULL, m_HandleInstance, NULL);
	if (m_WindowHandle == NULL)
	{
		return false;
	}

	// hide window until finished initializing
	Hide();

	return true;
}


void Window::Shutdown()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (m_Fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_WindowHandle);
	m_WindowHandle = NULL;

	// Remove the application instance.
	UnregisterClass(m_WindowName, m_HandleInstance);
}
