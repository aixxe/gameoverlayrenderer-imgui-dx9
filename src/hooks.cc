#include "hooks.h"

// Pull in the reference WndProc handler to handle window messages.
extern IMGUI_API LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

// Ensure 'original function' symbols are defined. (see hooks.h)	
decltype(hooks::original_present) hooks::original_present = nullptr;
decltype(hooks::original_reset) hooks::original_reset = nullptr;

// The main window handle of the game.
HWND game_hwnd = NULL;

// The original WndProc used by the game window.
WNDPROC game_wndproc = NULL;

// Used to find windows belonging to the game process.
BOOL CALLBACK find_game_hwnd(HWND hwnd, LPARAM game_pid) {
	// Skip windows not belonging to the game process.
	DWORD hwnd_pid = NULL;

	GetWindowThreadProcessId(hwnd, &hwnd_pid);

	if (hwnd_pid != game_pid)
		return TRUE;

	// Set the target window handle and stop the callback.
	game_hwnd = hwnd;

	return FALSE;
}

LRESULT STDMETHODCALLTYPE hooks::user_wndproc(HWND window, UINT message_type, WPARAM w_param, LPARAM l_param) {
	// You'll probably want to add a check here so it only calls the ImGui handler when the GUI is active..
	ImGui_ImplDX9_WndProcHandler(window, message_type, w_param, l_param);

	// ..but we'll be calling both WndProc functions here to keep things simple.
	return CallWindowProc(game_wndproc, window, message_type, w_param, l_param);
};

HRESULT STDMETHODCALLTYPE hooks::user_present(IDirect3DDevice9* thisptr, const RECT* src, const RECT* dest, HWND wnd_override, const RGNDATA* dirty_region) {
	// Perform general setup tasks when this function is called for the first time.
	static bool is_initialised = false;

	if (!is_initialised) {
		// Find a handle to the first top-level window belonging to the game process.
		EnumWindows(find_game_hwnd, GetCurrentProcessId());

		if (game_hwnd != NULL) {
			// Swap out the window message handler for our own, allowing us to intercept input events.
			game_wndproc = reinterpret_cast<WNDPROC>(
				SetWindowLongPtr(game_hwnd, GWLP_WNDPROC, LONG_PTR(hooks::user_wndproc))
			);

			// Perform final ImGui setup tasks and..
			ImGui_ImplDX9_Init(game_hwnd, thisptr);

			// ..we're all done!
			is_initialised = true;
		}
	} else {
		// Just the usual ImGui rendering stuff here.
		ImGui_ImplDX9_NewFrame();
		hooks::draw_interface();
		ImGui::Render();
	}
	
	// Call the original 'Present' function.
	return hooks::original_present(thisptr, src, dest, wnd_override, dirty_region);
}

HRESULT STDMETHODCALLTYPE hooks::user_reset(IDirect3DDevice9* thisptr, D3DPRESENT_PARAMETERS* params) {
	// Destroy and re-create device objects when the device resets.
	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui_ImplDX9_CreateDeviceObjects();

	// Call the original 'Reset' function.
	return hooks::original_reset(thisptr, params);
}