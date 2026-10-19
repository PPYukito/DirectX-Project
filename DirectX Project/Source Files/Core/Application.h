#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <vector>

//#include <wrl/client.h> // <-- for <Microsoft::WRL::ComPtr>, I want to manage the lifetime of COM objects manually, so no need for that now.

class Application
{

public:
	// Lifecycle (called from main in this order)
	bool Initialize();
	void Run();
	void Shutdown();

private:
#pragma region Window
	HWND m_Window = nullptr;
	bool m_IsRunning = false;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#pragma endregion

#pragma region DirectX
	// Created in InitDirectX() in THIS order, released in Shutdown() in REVERSE order
	ID3D11Device*			m_Device			= nullptr;	// Makes things (buffers, textures, shaders)
	ID3D11DeviceContext*	m_DeviceContext		= nullptr;	// Does things (clear, draw)
	IDXGISwapChain*			m_SwapChain			= nullptr;	// Holds the 2 canvases and flips them
	ID3D11RenderTargetView* m_RenderTargetView	= nullptr;	// "Paintable" label on the back buffer
	ID3D11VertexShader*		m_VertexShader		= nullptr;	// GPU program: where each corner goes
	ID3D11PixelShader*		m_PixelShader		= nullptr;	// GPU program: color of each pixel
	ID3D11InputLayout*		m_InputLayout		= nullptr;	// How to read one vertex from the buffer
	ID3D11Buffer*			m_VertexBuffer		= nullptr;	// The vertices themselves (GPU memory)

	UINT m_VertexCount = 0;

	float m_BgColor[4] = { 0.0f, 0.62f, 0.86f, 1.0f };		// RGBA

	// Setup (InitDirectX calls the others in order)
	bool InitDirectX();
	bool CreateDeviceAndSwapChain();
	bool CreateRenderTarget();
	bool LoadShaders();
	bool LoadShaderFile(const wchar_t* path, std::vector<char>& outBytes);
	bool CreateVertexBuffer();

	// Every frame
	void Render();
#pragma endregion

};
