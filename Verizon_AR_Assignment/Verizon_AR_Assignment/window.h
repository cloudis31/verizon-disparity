#pragma once

#include "pch.h"

class Window
{
public:
	inline Window(LPCWSTR _name) : m_WindowName(_name) {}
	Window(const Window& _other) = default;
	~Window() = default;

	bool Initialize(int _width, int _height, WNDPROC _wndproc, bool _fullscreen = false);
	void Shutdown();

	inline void Show() {
		ShowWindow(m_WindowHandle, SW_SHOW);
		SetForegroundWindow(m_WindowHandle);
		SetFocus(m_WindowHandle);
	}
	inline void Hide() { ShowWindow(m_WindowHandle, SW_HIDE); }

	inline HWND GetHWND() { return m_WindowHandle; }

private:
	LPCWSTR m_WindowName;
	HINSTANCE m_HandleInstance;
	HWND m_WindowHandle;
	bool m_Fullscreen;

public:
	static HWND CreateTempWindow();
	static void DestroyTempWindow();

private:
	static HWND m_TmpHwnd;
	static LPCWSTR m_TmpWinName;
	static HINSTANCE m_TmpHInstance;
};