#pragma once
#include "../Graphics/Graphics.h"
static Graphics* Renderer = nullptr;

class SceneRenderer :
    public Graphics
{
public:
    virtual ~SceneRenderer() {}
public:
    static Graphics* GetInstance();

    // Inherited via Graphics
    virtual bool Init() override;
    virtual void OnResize(uint width, uint height, bool minimized) override;
    virtual void OnDestroy() override;
    virtual void OnRender() override;
private:
    RECT WindowRect;
    ComPtr<ID3D12Device8> Device;
    ComPtr<ID3D12GraphicsCommandList6> CommandList;
};

