#include "dllmain.h"
#include "renderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace dutycore
{
    renderer::tPresent renderer::oPresent = {};
    bool renderer::initializedImgui = false;

    void renderer::SetupDXDevice()
    {
        auto tempWnd = CreateWindowExA(0,
            "BUTTON",
            "DX",
            WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            300,
            300,
            NULL,
            NULL,
            (HINSTANCE)GetModuleHandleA(NULL),
            NULL);

        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = tempWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = true;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        IDXGISwapChain* SwapChain;
        ID3D11Device* Device;
        ID3D11DeviceContext* DeviceCtx;

        auto featureLevels = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

        D3D_FEATURE_LEVEL featureLevel = (D3D_FEATURE_LEVEL)0;

        if (D3D11CreateDeviceAndSwapChain(
            NULL, 
            D3D_DRIVER_TYPE_HARDWARE, 
            NULL,
            0, 
            featureLevels.begin(),
            featureLevels.size(),
            D3D11_SDK_VERSION, 
            &sd, 
            &SwapChain, 
            &Device, 
            &featureLevel, 
            &DeviceCtx) != S_OK)
        {
            main::Log.WriteLine("^1ERROR: Failed to create D3D11 Device and SwapChain");
            CleanupDXDevice();
            return;
        }

        const auto vtable = *reinterpret_cast<void***>(SwapChain);
        renderer::oPresent = reinterpret_cast<tPresent>(vtable[8]);

        DWORD OldProtect;
        VirtualProtect(vtable, sizeof(void*) * 5, PAGE_EXECUTE_READWRITE, &OldProtect);
        vtable[8] = reinterpret_cast<void*>(renderer::OnPresent);
        VirtualProtect(vtable, sizeof(void*) * 5, OldProtect, &OldProtect);

        DestroyWindow(tempWnd);

        main::Log.WriteLine("Successfully Created DX Hook");
    }

    void renderer::CleanupDXDevice()
    {
        // TODO
    }

    char* base = (char*)GetModuleHandleA(NULL);
    auto showDemoWindow = false;
    ID3D11RenderTargetView* renderTarget = {};
    HRESULT renderer::OnPresent(IDXGISwapChain* chain, ID3D11Device** device, ID3D11DeviceContext** deviceCtx)
    {
        ID3D11Device* dxdevice = *(ID3D11Device**)(base + 0xf4b8700);
        ID3D11DeviceContext* dxcontext = *(ID3D11DeviceContext**)(base + 0xf4b8708);
        if (!initializedImgui)
        {
            initializedImgui = true;

            ID3D11Texture2D* backBuffer = {};
            chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
            if (!backBuffer)
            {
                main::Log.WriteLine("Failed to get BackBuffer");
                return oPresent(chain, device, deviceCtx);
            }
            main::Log.WriteLine("Got BackBuffer");

            D3D11_TEXTURE2D_DESC texture;
            backBuffer->GetDesc(&texture);
            main::Log.WriteLine("Got Texture");

            DXGI_SWAP_CHAIN_DESC sd;
            chain->GetDesc(&sd);
            main::Log.WriteLine("Got Swapchain Desc");

            D3D11_RENDER_TARGET_VIEW_DESC viewDesc = {
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
                .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D
            };

            dxdevice->CreateRenderTargetView(backBuffer, &viewDesc, &renderTarget);
            main::Log.WriteLine("Created Render Target View");

            backBuffer->Release();

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            main::Log.WriteLine("Created ImGui Context");
            ImGuiIO io = ImGui::GetIO();
            io.DisplaySize = ImVec2(texture.Width, texture.Height);
            ImGui::StyleColorsDark();

            ImGui_ImplWin32_Init(sd.OutputWindow);
            main::Log.WriteLine("ImGui ImplWin32 Init");
            ImGui_ImplDX11_Init(dxdevice, dxcontext);
            main::Log.WriteLine("ImGui ImplDX11 Init");
            main::Log.WriteLine("Initialized Dear ImGui");
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        // TODO: Replace with command list constructed from CSC
        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        dxcontext->OMSetRenderTargets(1, &renderTarget, NULL);

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        return renderer::oPresent(chain, device, deviceCtx);
    }
}
