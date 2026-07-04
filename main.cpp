#include <atomic>
#include <csignal>
#include <iostream>

#include "draw/ESP.h"
#include "draw/Overlay.h"
#include "gamestate/GameState.h"
#include "memory/memory.h"

inline std::atomic<bool> g_Running(true);
void SignalHandler(int signum) {g_Running = false;}

int main() {
    std::signal(SIGINT, SignalHandler);
    if (!InitOverlay()) {
        std::cerr << "[-] Failed to init Overlay (GLFW/Wayland)" << std::endl;
        return 1;
    }

    ProcessId = FindGamePID();
    if (!ProcessId) {
        std::cerr << "[-] Could not find game PID, is the game running?" << std::endl;
        return 0;
    }

    GameState gs = GameState(0x140000000);
    while (g_Running && !glfwWindowShouldClose(window)) {
        GSRet gsr = gs.tick();

        RenderBegin();

        char buf[64];
        sprintf(buf, "Entities: %lu", gsr.ents.size());
        DrawTextImGui(10, 10, IM_COL32(255, 0, 0, 255), buf);

        ESP(gsr.vm, gsr.ents, gsr.teamID);

        RenderEnd();
    }
    std::cout << "[+] Destructing Window" << std::endl;

    if (ImGui::GetCurrentContext()) {
        if (ImGui::GetIO().BackendRendererUserData)
            ImGui_ImplOpenGL3_Shutdown();

        if (ImGui::GetIO().BackendPlatformUserData)
            ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }

    if (window) {
        glfwHideWindow(window);
        glfwDestroyWindow(window);
    }
    return 0;
}
