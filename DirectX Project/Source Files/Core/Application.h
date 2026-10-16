#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

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
	// Created in InitDirectX(), released in Shutdown() (reverse order)
	ID3D11Device* m_Device						= nullptr;	// Makes things (buffers, textures, shaders)
	ID3D11DeviceContext* m_DeviceContext		= nullptr;	// Does things (clear, draw)
	IDXGISwapChain* m_SwapChain					= nullptr;	// Holds the 2 canvases and flips them
	ID3D11RenderTargetView* m_RenderTargetView	= nullptr;	// "Paintable" label on the back buffer

	float bgColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; // RGBA

	bool InitDirectX();
	void Render();
#pragma endregion

};
