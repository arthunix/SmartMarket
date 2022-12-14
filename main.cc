// Dear ImGui: standalone example application for DirectX 12
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// Important: to compile on 32-bit systems, the DirectX12 backend requires code to be compiled with '#define ImTextureID ImU64'.
// This is because we need ImTextureID to carry a 64-bit value and by default ImTextureID is defined as void*.
// This define is set in the example .vcxproj file and need to be replicated in your app or by adding it to your imconfig.h file.

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <ctime>
#include <iostream>
#include <vector>

#include "Market.h"
#include "imguidatechooser.h"

#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif // !_DEBUG

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

struct FrameContext
{
    ID3D12CommandAllocator* CommandAllocator;
    UINT64                  FenceValue;
};

// Data
static int const                    NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT                         g_frameIndex = 0;

static int const                    NUM_BACK_BUFFERS = 3;
static ID3D12Device*                g_pd3dDevice = NULL;
static ID3D12DescriptorHeap*        g_pd3dRtvDescHeap = NULL;
static ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = NULL;
static ID3D12CommandQueue*          g_pd3dCommandQueue = NULL;
static ID3D12GraphicsCommandList*   g_pd3dCommandList = NULL;
static ID3D12Fence*                 g_fence = NULL;
static HANDLE                       g_fenceEvent = NULL;
static UINT64                       g_fenceLastSignaledValue = 0;
static IDXGISwapChain3*             g_pSwapChain = NULL;
static HANDLE                       g_hSwapChainWaitableObject = NULL;
static ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext* WaitForNextFrameResources();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    //HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX12 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX12 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    Market<10, 10, 10> mkt_container;

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
        DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
        g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
        g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
#ifdef _DEBUG
    bool show_demo_window = true;
#endif // _DEBUG
    bool show_another_window = false;
    bool show_credits = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

#ifdef _DEBUG
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
#endif // _DEBUG
        //std::cout << sizeof(Product) << std::endl;
        //std::cout << sizeof(structio_t) << std::endl;
        
        {
            ImGui::Begin("Smart Market - Main Window");

            if (ImGui::CollapsingHeader("Search"))
            {
                static char sNameToSearch[NAME_SIZE] = "";
                static bool sShowItemsFoundInSearch = false;

                ImGui::InputTextWithHint("Find", "The name of the product to search", sNameToSearch, IM_ARRAYSIZE(sNameToSearch));
                ImGui::SameLine();

                if (ImGui::Button("Search On Market"))
                {
                    if (sShowItemsFoundInSearch == true)
                    {
                        sShowItemsFoundInSearch = false;
                    }
                    else
                    {
                        sShowItemsFoundInSearch = true;
#ifdef _DEBUG
                        std::cout << "TRACE RING 3 : SEARCHING FOR VALUES IN SEARCH : " << std::endl;
#endif // _DEBUG
                    }
                }

                if (sShowItemsFoundInSearch)
                {
                    static int sCurrentItemInSearch = 1;

                    std::vector<rbnode*> fd = mkt_container.LookupForAProducts(sNameToSearch);

                    ImGui::ListBox(
                        "ListBox Of Found In Search",
                        &sCurrentItemInSearch,
                        VectorOfStructIOCharGetter,
                        fd.data(), 
                        (int)fd.size()
                    );

#ifdef _DEBUG
                    std::cout << "TRACE RING 3 : LISTING VALUES IN SEARCH : " << std::endl;
#endif // _DEBUG
                }
            }

            if (ImGui::CollapsingHeader("Insert"))
            {
                static char sNameToInsert[NAME_SIZE] = "";
                static char sDescriptToInsert[DESCRIPTION_SIZE] = "";
                static char sBrandToInsert[BRAND_SIZE] = "";
                static tm sExpirationDateToInsert = ImGui::GetDateZero();
                static tm sManufacturingDateToInsert = ImGui::GetDateZero();
                static int sLoteToInsert = 12345;
                static double sPriceToInsert = 100.00;

                ImGui::InputTextWithHint("Name", "The name of the product", sNameToInsert, IM_ARRAYSIZE(sNameToInsert));
                ImGui::InputTextWithHint("Description", "The description of the product", sDescriptToInsert, IM_ARRAYSIZE(sDescriptToInsert));
                ImGui::InputTextWithHint("Brand", "The brand of the product", sBrandToInsert, IM_ARRAYSIZE(sBrandToInsert));
                ImGui::DateChooser("Choose Expiration Date", sExpirationDateToInsert, "%m/%d/%Y", false, (bool*)0);
                ImGui::DateChooser("Choose Manufacturing Date", sManufacturingDateToInsert, "%m/%d/%Y", false, (bool*)0);
                ImGui::InputInt("Choose The Lote", &sLoteToInsert);
                ImGui::InputDouble("Choose The Price US$", &sPriceToInsert);

                if (ImGui::Button("Push"))
                {
                    Product sProductWillBePushed(
                        sNameToInsert, sDescriptToInsert, 
                        sBrandToInsert,
                        mktime(&sExpirationDateToInsert), 
                        mktime(&sManufacturingDateToInsert), 
                        sLoteToInsert, 
                        sPriceToInsert
                    );
#ifdef _DEBUG
                    sProductWillBePushed.printProduct();
                    std::cout << "TRACE RING 3 : INSERT - PUSHED " << std::endl;
#endif // _DEBUG

                    mkt_container.insertProduct(sProductWillBePushed);
                }

#ifdef _DEBUG
                std::cout << "TRACE RING 3 : NAME AT INSERT         : " << sNameToInsert << std::endl;
                std::cout << "TRACE RING 3 : DESCRIPTION AT INSERT  : " << sDescriptToInsert << std::endl;
                std::cout << "TRACE RING 3 : BRAND AT INSERT        : " << sBrandToInsert << std::endl;
                std::cout << "TRACE RING 3 : PRICE AT INSERT        : " << sPriceToInsert << std::endl;
                std::cout << "TRACE RING 3 : LOTE AT INSERT         : " << sLoteToInsert << std::endl;

                char buff[80];
                asctime_s(buff, sizeof(buff), &sExpirationDateToInsert);
                std::cout << "TRACE RING 3 : EXP. DATE AT INSERT    : " << buff << std::endl;
                asctime_s(buff, sizeof(buff), &sManufacturingDateToInsert);
                std::cout << "TRACE RING 3 : MAN. DATE AT INSERT    : " << buff << std::endl;
#endif // _DEBUG
            }

            if (ImGui::CollapsingHeader("Remove"))
            {
                static char sNameToRemove[NAME_SIZE] = "";
                static bool sShowItemsFoundInRemove = false;

                ImGui::InputTextWithHint("Remove Of Market", "The name of the product to remove", sNameToRemove, IM_ARRAYSIZE(sNameToRemove));
                ImGui::SameLine();
                if (ImGui::Button("Delete"))
                {
                    if (sShowItemsFoundInRemove == true)
                    {
                        sShowItemsFoundInRemove = false;
                    }
                    else
                    {
                        sShowItemsFoundInRemove = true;
#ifdef _DEBUG
                        std::cout << "TRACE RING 3 : SEARCHING FOR VALUES IN REMOVE : " << std::endl;
#endif // _DEBUG
                    }
                }

                if (sShowItemsFoundInRemove) {
                    static int sCurrentItemInRemove = 1;

                    std::vector<rbnode*> fd = mkt_container.LookupForAProducts(sNameToRemove);

                    ImGui::ListBox(
                        "ListBox Of Found In Remove",
                        &sCurrentItemInRemove,
                        VectorOfStructIOCharGetter,
                        fd.data(),
                        (int)fd.size()
                    );
#ifdef _DEBUG
                    std::cout << "TRACE RING 3 : LISTING VALUES IN REMOVE : " << std::endl;
#endif // _DEBUG
                    if (ImGui::Button("Sure Remove Selected"))
                    {
#ifdef _DEBUG
                        std::cout << "TRACE RING 3 : REMOVING PRODUCT : " << std::endl;
                        //structio_t_print(fd[sCurrentItemInRemove]);
#endif // _DEBUG
                        mkt_container.removeProduct(fd.data(), sCurrentItemInRemove);
                    }
                }
            }

            if (ImGui::CollapsingHeader("Change"))
            {
                static char sNameToChange[NAME_SIZE] = "";
                static bool sShowItemsFoundInChange = false;

                ImGui::InputTextWithHint("Change On Market", "The name of the product to change", sNameToChange, IM_ARRAYSIZE(sNameToChange));
                ImGui::SameLine();
                if (ImGui::Button("Modify"))
                {
                    if (sShowItemsFoundInChange == true)
                    {
                        sShowItemsFoundInChange = false;
                    }
                    else
                    {
                        sShowItemsFoundInChange = true;
                    }
                }

                if (sShowItemsFoundInChange) {
                    static int sCurrentItemInChange = 1;

                    std::vector<rbnode*> fd = mkt_container.LookupForAProducts(sNameToChange);

                    ImGui::ListBox(
                        "ListBox Of Found In Remove",
                        &sCurrentItemInChange,
                        VectorOfStructIOCharGetter,
                        fd.data(),
                        (int)fd.size()
                    );
#ifdef _DEBUG
                    std::cout << "TRACE RING 3 : LISTING VALUES IN CHANGE : " << std::endl;
#endif // _DEBUG
                    static char sNameToChange[NAME_SIZE] = "";
                    static char sDescriptToChange[DESCRIPTION_SIZE] = "";
                    static char sBrandToChange[BRAND_SIZE] = "";
                    static tm sExpirationDateToChange = ImGui::GetDateZero();
                    static tm sManufacturingDateToChange = ImGui::GetDateZero();
                    static int sLoteToChange = 12345;
                    static double sPriceToChange = 100.00;

                    ImGui::InputTextWithHint("Name", "The name of the product", sNameToChange, IM_ARRAYSIZE(sNameToChange));
                    ImGui::InputTextWithHint("Description", "The description of the product", sDescriptToChange, IM_ARRAYSIZE(sDescriptToChange));
                    ImGui::InputTextWithHint("Brand", "The brand of the product", sBrandToChange, IM_ARRAYSIZE(sBrandToChange));
                    ImGui::DateChooser("Choose Expiration Date", sExpirationDateToChange, "%m/%d/%Y", false, (bool*)0);
                    ImGui::DateChooser("Choose Manufacturing Date", sManufacturingDateToChange, "%m/%d/%Y", false, (bool*)0);
                    ImGui::InputInt("Choose The Lote", &sLoteToChange);
                    ImGui::InputDouble("Choose The Price US$", &sPriceToChange);

                    if (ImGui::Button("Sure Change Selected"))
                    {
                        Product sProductWillBePushed(
                            sNameToChange, sDescriptToChange,
                            sBrandToChange,
                            mktime(&sExpirationDateToChange),
                            mktime(&sManufacturingDateToChange),
                            sLoteToChange,
                            sPriceToChange
                        );
#ifdef _DEBUG
                        sProductWillBePushed.printProduct();
                        std::cout << "TRACE RING 3 : CHANGE - PUSHED " << std::endl;
#endif // _DEBUG
                        mkt_container.changeProduct(fd.data(), sCurrentItemInChange, sProductWillBePushed);
                    }
                }
            }

            if (show_credits)
            {
                ImGui::Begin("Credits", &show_another_window);
                ImGui::Text("Copyright (c) 2022, Arthur Eugenio Silverio. All rights reserved.");
                ImGui::Text("Copyright (c) 2022 Caroline Elisa Duarte de Souza. All rights reserved.");
                ImGui::Text("Copyright (c) 2022 Anderson Antonio de Melo. All rights reserved.");
                ImGui::Text("BSD 2-Clause Simplified License");
                ImGui::Text("https://github.com/arthunix/SmartMarket");
                if (ImGui::Button("Close Credits"))
                    show_credits = false;
                ImGui::End();
            }

            if (ImGui::Button("Show Credits")) show_credits = (show_credits == true) ? false : true;
            ImGui::ColorEdit3("Change Background Color", (float*)&clear_color); // Edit 3 floats representing a color
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

#ifdef _DEBUG

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

#endif // _DEBUG

        // Rendering
        ImGui::Render();

        FrameContext* frameCtx = WaitForNextFrameResources();
        UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
        frameCtx->CommandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource   = g_mainRenderTargetResource[backBufferIdx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
        g_pd3dCommandList->Reset(frameCtx->CommandAllocator, NULL);
        g_pd3dCommandList->ResourceBarrier(1, &barrier);

        // Render Dear ImGui graphics
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, NULL);
        g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
        g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
        g_pd3dCommandList->ResourceBarrier(1, &barrier);
        g_pd3dCommandList->Close();

        g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync

        UINT64 fenceValue = g_fenceLastSignaledValue + 1;
        g_pd3dCommandQueue->Signal(g_fence, fenceValue);
        g_fenceLastSignaledValue = fenceValue;
        frameCtx->FenceValue = fenceValue;
    }

    WaitForLastSubmittedFrame();

    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC1 sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
    }

    // [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
    ID3D12Debug* pdx12Debug = NULL;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
        pdx12Debug->EnableDebugLayer();
#endif

    // Create device
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
        return false;

    // [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
    if (pdx12Debug != NULL)
    {
        ID3D12InfoQueue* pInfoQueue = NULL;
        g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        pInfoQueue->Release();
        pdx12Debug->Release();
    }
#endif

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
            return false;

        SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        {
            g_mainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
            return false;
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
            return false;
    }

    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
            return false;

    if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
        g_pd3dCommandList->Close() != S_OK)
        return false;

    if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
        return false;

    g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (g_fenceEvent == NULL)
        return false;

    {
        IDXGIFactory4* dxgiFactory = NULL;
        IDXGISwapChain1* swapChain1 = NULL;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
            return false;
        if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK)
            return false;
        if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
            return false;
        swapChain1->Release();
        dxgiFactory->Release();
        g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
        g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
    }

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->SetFullscreenState(false, NULL); g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_hSwapChainWaitableObject != NULL) { CloseHandle(g_hSwapChainWaitableObject); }
    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = NULL; }
    if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = NULL; }
    if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = NULL; }
    if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = NULL; }
    if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = NULL; }
    if (g_fence) { g_fence->Release(); g_fence = NULL; }
    if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }

#ifdef DX12_ENABLE_DEBUG_LAYER
    IDXGIDebug1* pDebug = NULL;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
    {
        pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
        pDebug->Release();
    }
#endif
}

void CreateRenderTarget()
{
    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource* pBackBuffer = NULL;
        g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
        g_mainRenderTargetResource[i] = pBackBuffer;
    }
}

void CleanupRenderTarget()
{
    WaitForLastSubmittedFrame();

    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = NULL; }
}

void WaitForLastSubmittedFrame()
{
    FrameContext* frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue == 0)
        return; // No fence was signaled

    frameCtx->FenceValue = 0;
    if (g_fence->GetCompletedValue() >= fenceValue)
        return;

    g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
    WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext* WaitForNextFrameResources()
{
    UINT nextFrameIndex = g_frameIndex + 1;
    g_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, NULL };
    DWORD numWaitableObjects = 1;

    FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue != 0) // means no fence was signaled
    {
        frameCtx->FenceValue = 0;
        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        waitableObjects[1] = g_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtx;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            WaitForLastSubmittedFrame();
            CleanupRenderTarget();
            HRESULT result = g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
            assert(SUCCEEDED(result) && "Failed to resize swapchain.");
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
