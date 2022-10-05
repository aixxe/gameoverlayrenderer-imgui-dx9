/*

	gameoverlayrenderer_imgui_dx9 - ImGui via the Steam overlay renderer.
	Copyright (C) 2017, aixxe. <me@aixxe.net>

	For more information, see https://aixxe.net/2017/09/steam-overlay-rendering.

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

*/

#include "hooks.h"
#include "findpattern.h"

// Convenience function invoked in the 'Present' hook. (see hooks.cc)
FORCEINLINE void hooks::draw_interface() {
	// Generic text that will appear in the Debug window.
	ImGui::Text("Hello world from the Steam Overlay!");

	// Top-left framerate display overlay window.
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::Begin("FPS", nullptr, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::End();
}

void STDMETHODCALLTYPE gameoverlayrenderer_imgui_dx9_init() {
	// Perform signature scans inside the 'gameoverlayrenderer.dll' library.
	std::uintptr_t present_addr = FindPattern("gameoverlayrenderer.dll", "FF 15 ? ? ? ? 8B F8 85 DB") + 2;
	std::uintptr_t reset_addr = FindPattern("gameoverlayrenderer.dll", "C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B F8") + 9;

	// Store the original contents of the pointers for later usage.
	hooks::original_present = **reinterpret_cast<decltype(&hooks::original_present)*>(present_addr);
	hooks::original_reset = **reinterpret_cast<decltype(&hooks::original_reset)*>(reset_addr);

	// Switch the contents to point to our replacement functions.
	**reinterpret_cast<void***>(present_addr) = reinterpret_cast<void*>(&hooks::user_present);
	**reinterpret_cast<void***>(reset_addr) = reinterpret_cast<void*>(&hooks::user_reset);
}

BOOL WINAPI DllMain(HINSTANCE dll_instance, DWORD call_reason, LPVOID reserved) {
	DisableThreadLibraryCalls(dll_instance);

	if (call_reason == DLL_PROCESS_ATTACH)
		CreateThread(0, 0, LPTHREAD_START_ROUTINE(gameoverlayrenderer_imgui_dx9_init), 0, 0, 0);

	return TRUE;
}