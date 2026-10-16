#include "Application.h"

#pragma region Lifecycle

bool Application::Initialize()
{
	// --- Register the window class ---
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

	// --- Create the window ---
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

	// --- Set up DirectX (needs the window to exist) ---
	if (!InitDirectX())
		return false;

	m_IsRunning = true;

	return true;
}

void Application::Run()
{
	MSG msg{};

	// Main loop: one turn = one frame
	while (m_IsRunning)
	{
		// --- Handle all waiting window messages ---
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

		if (m_IsRunning)
		{
			Render();
		}
	}
}

void Application::Shutdown()
{
	// --- Release DirectX objects (reverse order of creation) ---
	{
		if (m_RenderTargetView)
			m_RenderTargetView->Release();

		if (m_SwapChain)
			m_SwapChain->Release();

		if (m_DeviceContext)
			m_DeviceContext->Release();

		if (m_Device)
			m_Device->Release();

		m_RenderTargetView = nullptr;
		m_SwapChain = nullptr;
		m_DeviceContext = nullptr;
		m_Device = nullptr;
	}

	// --- Destroy the window (created first, destroyed last) ---
	if (m_Window)
	{
		DestroyWindow(m_Window);
		m_Window = nullptr;
	}
}

#pragma endregion

#pragma region Window

LRESULT Application::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// User clicked X: ask before closing
		case WM_CLOSE:
		{
			int result = MessageBox(hwnd, L"Are you sure you want to exit?", L"Exit Confirmation", MB_YESNO | MB_ICONQUESTION);

			if (result == IDYES)
				DestroyWindow(hwnd);

			return 0;
		}

		// Window is gone: tell the main loop to stop (sends WM_QUIT)
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
			return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
}

#pragma endregion

#pragma region DirectX

bool Application::InitDirectX()
{
	// --- 1. Describe the swap chain (2 window-sized canvases that flip) ---
	DXGI_SWAP_CHAIN_DESC scd{};
	scd.BufferDesc.Width = 0;									// 0 = match the window
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// RGBA, 8 bits each
	scd.BufferCount = 2;										// One on screen, one to draw into
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			// We draw into these
	scd.OutputWindow = m_Window;
	scd.Windowed = TRUE;
	scd.SampleDesc.Count = 1;									// No anti-aliasing
	scd.SampleDesc.Quality = 0;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;				// Flip, redraw everything each frame

	// --- 2. Create device, context and swap chain ---
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,					// Default GPU
		D3D_DRIVER_TYPE_HARDWARE,	// Use the real GPU
		nullptr,					// Software driver (unused)
		0,							// Flags
		nullptr,					// Feature levels: default list
		0,							// Feature level count
		D3D11_SDK_VERSION,
		&scd,
		&m_SwapChain,				// [out]
		&m_Device,					// [out]
		nullptr,					// [out] Feature level we got (don't care)
		&m_DeviceContext			// [out]
	);

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create DirectX device and swap chain.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// --- 3. Get the back buffer (the canvas we draw into) ---
	ID3D11Texture2D* backBuffer = nullptr;
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to get back buffer.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// --- 4. Create the render target view (make the canvas "paintable") ---
	hr = m_Device->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);

	// The view keeps its own reference, so we're done with ours (release before the check, so no leak on failure)
	backBuffer->Release();
	backBuffer = nullptr;

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create render target view.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

void Application::Render()
{
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, bgColor);
	m_SwapChain->Present(1, 0);
}

#pragma endregion
