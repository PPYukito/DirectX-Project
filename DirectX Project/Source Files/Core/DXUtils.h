#pragma once

// Small helpers for working with DirectX (COM) objects by hand.

// Release a COM object and set the pointer to nullptr.
// Safe to call on nullptr. Takes T*& (a reference to the pointer) so it can reset the caller's pointer.
template<typename T>
void SafeRelease(T*& ptr)
{
	if (ptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}
