#pragma once
#include "../Templates/TemplateClasses.h"
interface IDeviceNotify
{
    virtual void OnDeviceLost() = 0;
    virtual void OnDeviceRestored() = 0;
};
class D3D12DeviceResource
{
    friend class Graphics;
public:
    D3D12DeviceResource(
        DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT depthStencilBufferFormat = DXGI_FORMAT_D32_FLOAT,
        uint backBufferCount = 2,
        D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_11_0,
        uint flag = 0,
        uint adapterIDOverride = UINT_MAX
    );
    virtual ~D3D12DeviceResource() { WaitForGPU(); }

public:
    bool OnResize(int w, int h, bool minimized);
    void RegisterDeviceNotify(IDeviceNotify* Notifier);
    void InitDXGIAdapter();
    void CreateDeviceResources();
    void CreateWindowDependentResources();

    void WaitForGPU()noexcept;
    void ExecuteCommandLists();
    void HandleDeviceLost();

    void BeginFrame(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_PRESENT);
    void EndFrame(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET);

public:
    bool IsVisiable()const { return IsWindowVisible; }
    RECT GetOutputSize() const { return OutputSize; }
    uint GetSelectedResolution()const { return SupportedResolutions[SelectedResolution]; }
    uint GetSelectedResolutionIndex()const { return SelectedResolution; }
    uint GetBackBufferCount()const { return NumBackBuffer; }
public:
    CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView()const
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), BackBufferIndex, rtvDescriptorSize);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView()const
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    }
private:
    void MoveToNextFrame();
    void InitAdapter();
public:
    // Getters
    //IDXGIAdapter1*              GetAdapter()const { return Adapter.Get(); }
    ID3D12Device*               GetDevice()const { return Device.Get(); }
    ID3D12CommandQueue*         GetCommandQueue()const { return RenderCommandQueue.Get(); }
    ID3D12GraphicsCommandList*  GetCommandList()const { return CommandList.Get(); }
    ID3D12CommandAllocator*     GetCommandAllocator() { return CommandAllocator[BackBufferIndex].Get(); };

    IDXGIFactory4*              GetFactory()const { return Factory4.Get(); }
    IDXGISwapChain3*            GetSwapChain()const { return SwapChain.Get(); }
    ID3D12Resource*             GetRenderTarget() const{ return RenderTargets[BackBufferIndex].Get(); }
    ID3D12Resource*             GetDepthStencil()const { return DepthStencil.Get(); }
    D3D12_VIEWPORT              GetViewport()const { return ScreenViewport; }
    D3D12_RECT                  GetScissorRect()const { return ScissorRect; }
    DXGI_FORMAT                 GetBackBufferFormat()const { return BackBufferFormat; }
    DXGI_FORMAT                 GetDepthStencilBufferFormat()const { return DepthStencilBufferFormat; }
    uint                        GetCurrentFrameIndex()const { return BackBufferIndex; }
    uint                        GetPreviousFrameIndex()const { return BackBufferIndex == 0 ? NumBackBuffer - 1 : BackBufferIndex - 1; }
    D3D_FEATURE_LEVEL           GetFeatureLevel()const { return MaxFeatureLevel; }
private:
    const static size_t MAX_BACK_BUFFER_COUNT = 3;

    uint AdapterIDOverride;
    uint AdapterID;
    uint BackBufferIndex;
    std::vector<ComPtr<IDXGIAdapter1>>  Adapters;
    std::vector<ComPtr<IDXGIOutput>>    Outputs;
    ComPtr<ID3D12Device>                Device;
    ComPtr<ID3D12CommandQueue>          RenderCommandQueue;
    ComPtr<ID3D12GraphicsCommandList>   CommandList;
    ComPtr<ID3D12CommandAllocator>      CommandAllocator[MAX_BACK_BUFFER_COUNT];

    ComPtr<IDXGIFactory4>               Factory4;
    ComPtr<IDXGISwapChain3>             SwapChain;
    ComPtr<ID3D12Resource>              RenderTargets[MAX_BACK_BUFFER_COUNT];
    ComPtr<ID3D12Resource>              DepthStencil;

    ComPtr<ID3D12Fence>                 Fence;
    UINT64                              FenceValue[MAX_BACK_BUFFER_COUNT];
    Microsoft::WRL::Wrappers::Event     FenceEvent;

    ComPtr<ID3D12DescriptorHeap>        RTVDescriptorHeap;
    ComPtr<ID3D12DescriptorHeap>        DSVDescriptorHeap;
    uint                                rtvDescriptorSize = -1;

    D3D12_VIEWPORT                      ScreenViewport = {};
    D3D12_RECT                          ScissorRect = {};
    RECT                                OutputSize;

    DXGI_FORMAT                         BackBufferFormat;
    DXGI_FORMAT                         DepthStencilBufferFormat;
    UINT                                NumBackBuffer;
    D3D_FEATURE_LEVEL                   MinFeatureLevel;
    D3D_FEATURE_LEVEL                   MaxFeatureLevel = (D3D_FEATURE_LEVEL)0;

    uint                                Options = 0;
    uint                                SelectedResolution = 0;
    std::vector<uint>                   SupportedResolutions;
    IDeviceNotify*                      DeviceNotify = nullptr;
    std::wstring                        AdapterDescription;
    bool                                IsWindowVisible = false;
};

