#include "Application.h"

LRESULT Application::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
		{
			int result = MessageBox(hwnd, L"Are you sure you want to exit?", L"Exit Confirmation", MB_YESNO | MB_ICONQUESTION);
			
			if (result == IDYES)
				DestroyWindow(hwnd);

			return 0;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
			return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
}

bool Application::Initialize()
{
	// Initialization
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = L"DirectXWindowClass";
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	if (!RegisterClass(&wc))
	{
		MessageBox(nullptr, L"Failed to register window class.", L"Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	// Create the window
	m_Window = CreateWindowEx(
		0,
		wc.lpszClassName,
		L"DirectX Application",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1280,
		720,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	if (!m_Window)
	{
		MessageBox(nullptr, L"Failed to create window.", L"Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	ShowWindow(m_Window, SW_SHOW);
	m_IsRunning = true;

	return true;
}

void Application::Run()
{
	MSG msg{};

	while (m_IsRunning)
	{
		// Main loop
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				m_IsRunning = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
}

void Application::Shutdown()
{
	if (m_Window)
	{
		DestroyWindow(m_Window);
		m_Window = nullptr;
	}
}