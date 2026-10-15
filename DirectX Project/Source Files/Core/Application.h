#pragma once

#include <Windows.h>

class Application
{

private:
	HWND m_Window = nullptr;
	bool m_IsRunning = false;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	bool Initialize();
	void Run();
	void Shutdown();

};