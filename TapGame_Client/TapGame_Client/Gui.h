#pragma once

#include "Client.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Gui
{
private:    
    ID3D11Device* g_pd3dDevice = nullptr;
    ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
    IDXGISwapChain* g_pSwapChain = nullptr;
    bool                     g_SwapChainOccluded = false;
    UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
    ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
    HWND hwnd;
    WNDCLASSEXW wc;

    Client& _client;


private:
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void Cleanup();

public:
    Gui(Client& client);
    void Run();
};