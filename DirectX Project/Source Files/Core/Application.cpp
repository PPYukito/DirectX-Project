#include "Application.h"
#include "DXUtils.h"

#include <fstream>

// ---------------------------------------------------------------
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

		// --- Draw the frame ---
		if (m_IsRunning)
		{
			Render();
		}
	}
}

void Application::Shutdown()
{
	// --- Release DirectX objects (exact reverse of the member list in Application.h) ---
	SafeRelease(m_VertexBuffer);
	SafeRelease(m_InputLayout);
	SafeRelease(m_PixelShader);
	SafeRelease(m_VertexShader);
	SafeRelease(m_RenderTargetView);
	SafeRelease(m_SwapChain);
	SafeRelease(m_DeviceContext);
	SafeRelease(m_Device);

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


#pragma region DirectX - Setup

bool Application::InitDirectX()
{
	// Each step needs the one before it, so stop at the first failure
	if (!CreateDeviceAndSwapChain())
		return false;

	if (!CreateRenderTarget())
		return false;

	if (!LoadShaders())
		return false;

	if (!CreateVertexBuffer())
		return false;

	return true;
}

bool Application::CreateDeviceAndSwapChain()
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

	return true;
}

bool Application::CreateRenderTarget()
{
	// --- 1. Get the back buffer (the canvas we draw into) ---
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to get back buffer.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// --- 2. Create the render target view (make the canvas "paintable") ---
	hr = m_Device->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);

	// The view keeps its own reference, so we're done with ours (release before the check, so no leak on failure)
	SafeRelease(backBuffer);

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create render target view.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

bool Application::LoadShaders()
{
	// --- 1. Load the vertex shader ---
	// vsBytes is kept until the end of this function: the input layout (step 3) needs it too
	std::vector<char> vsBytes;
	if (!LoadShaderFile(L"Shaders\\VertexShader.cso", vsBytes))
	{
		MessageBox(nullptr, L"Failed to load vertex shader.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	HRESULT hr = m_Device->CreateVertexShader(vsBytes.data(), vsBytes.size(), nullptr, &m_VertexShader);

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create vertex shader.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// --- 2. Load the pixel shader ---
	std::vector<char> psBytes;
	if (!LoadShaderFile(L"Shaders\\PixelShader.cso", psBytes))
	{
		MessageBox(nullptr, L"Failed to load pixel shader.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hr = m_Device->CreatePixelShader(psBytes.data(), psBytes.size(), nullptr, &m_PixelShader);

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create pixel shader.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// --- 3. Create the input layout (tell the GPU how to read our vertex data) ---
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_Device->CreateInputLayout(
		layoutDesc,
		1,
		vsBytes.data(),
		vsBytes.size(),
		&m_InputLayout
	);

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create input layout.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

bool Application::CreateVertexBuffer()
{
	struct Vertex
	{
		float x;
		float y;
	};

	Vertex vertices[] =
	{
		{  0.0f,    0.5f },   // top
		{  0.325f, -0.5f },   // bottom right  (0.577 (tan60) * (1080 / 1920), aspect corrected)
		{ -0.325f, -0.5f },   // bottom left
	};

	// --- Describe the buffer (size, usage, bind flags) ---
	D3D11_BUFFER_DESC bufferDesc{
		sizeof(vertices),
		D3D11_USAGE_IMMUTABLE,
		D3D11_BIND_VERTEX_BUFFER,
		0
	};

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = vertices;

	HRESULT hr = m_Device->CreateBuffer(&bufferDesc, &initData, &m_VertexBuffer);

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create vertex buffer.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_VertexCount = ARRAYSIZE(vertices);

	// Return
	return true;
}

bool Application::LoadShaderFile(const wchar_t* path, std::vector<char>& outBytes)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	if (!file)
		return false;

	std::streamsize size = file.tellg();

	file.seekg(0);

	outBytes.resize(size);

	file.read(outBytes.data(), size);

	return true;
}

#pragma endregion


#pragma region DirectX - Every frame

void Application::Render()
{
	// --- Viewport: draw into the whole client area ---
	RECT rect{};
	if (!GetClientRect(m_Window, &rect))
		return;

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(rect.right - rect.left);
	viewport.Height = static_cast<float>(rect.bottom - rect.top);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_DeviceContext->RSSetViewports(1, &viewport);

	// --- Plug in the canvas (Present unplugs it every frame with the flip model) ---
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);

	// --- Clear, (draw), present ---
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, m_BgColor);

	// --- Draw the triangle ---
	// Input Assembler (IA): the first pipeline stage, it reads vertices out of the vertex buffer.
	// Tell it how ONE vertex looks (the layout) and where the vertices are (the buffer).
	m_DeviceContext->IASetInputLayout(m_InputLayout);
	UINT stride = sizeof(float) * 2; // 2 floats per vertex (x, y): jump 8 bytes to reach the next vertex
	UINT offset = 0;                 // start reading at the beginning of the buffer
	m_DeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset); // slot 0, 1 buffer

	// How to connect the vertices: every 3 vertices = 1 separate triangle
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Which GPU programs to run: vertex shader (where each corner goes), pixel shader (color of each pixel)
	m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
	m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);

	// Go: run the whole pipeline with everything set above
	m_DeviceContext->Draw(m_VertexCount, 0); // Draw the specified number of vertices, starting from vertex 0

	m_SwapChain->Present(1, 0);
}

#pragma endregion
