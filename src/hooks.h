#pragma once

#include <d3d9.h>

#include <imgui/imgui.h>
#include <imgui_impl_dx9/imgui_impl_dx9.h>

namespace hooks {
	// The 'original' Present and Reset functions that will be called at the end of our hooks.
	extern HRESULT (STDMETHODCALLTYPE *original_present) (IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
	extern HRESULT (STDMETHODCALLTYPE *original_reset) (IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

	// Our replacement functions that will be put in place upon library load.
	extern HRESULT (STDMETHODCALLTYPE user_present) (IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
	extern HRESULT (STDMETHODCALLTYPE user_reset) (IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

	// Window message handling function.
	extern LRESULT (STDMETHODCALLTYPE user_wndproc) (HWND, UINT, WPARAM, LPARAM);

	// Convenience function for drawing the graphical interface.
	extern FORCEINLINE void (draw_interface) ();
};