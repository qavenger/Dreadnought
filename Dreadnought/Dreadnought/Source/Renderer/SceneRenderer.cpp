#include "pch.h"
#include "SceneRenderer.h"
#include "Timer.h"
#include "../Graphics/VertexFactory.h"
Graphics* SceneRenderer::GetInstance()
{
    if (!Renderer)
    {
        Renderer = new SceneRenderer();
        Graphics::SetInstance(Renderer);
    }
    return Renderer;
}

bool SceneRenderer::Init()
{
    Graphics::Init();
    ThrowIfFailed(DeviceResources->GetDevice()->QueryInterface(IID_PPV_ARGS(&Device)));
    ThrowIfFailed(DeviceResources->GetCommandList()->QueryInterface(IID_PPV_ARGS(&CommandList)));
    return true;
}

void SceneRenderer::OnResize(uint width, uint height, bool minimized)
{
    Graphics::OnResize(width, height, minimized);
    WindowRect = DeviceResources->GetOutputSize();
}

void SceneRenderer::OnDestroy()
{
    Graphics::OnDestroy();
    if (Renderer)
    {
        delete Renderer;
    }
}

void SceneRenderer::OnRender()
{
    float color[] = { (float)sin(g_Timer.Elapsed()),0,0,1 };
    CommandList->ClearRenderTargetView(DeviceResources->GetRenderTargetView(),
        color,
        1,
        &WindowRect
    );
    
    using namespace DirectX;
    VertexFactory::VertexLayout layout;
    layout.AppendElement(VertexFactory::Position3D).
        AppendElement(VertexFactory::TextureCoord).
        AppendElement(VertexFactory::Normal);
    static VertexFactory::VertexBuffer vb(std::move(layout));
    vb.EmplaceBack(XMFLOAT3( 0.0f, 0.0f, 1.0f), XMFLOAT2(0.5f, 0.0f), XMFLOAT3(0, 0, 1));
    vb.EmplaceBack(XMFLOAT3( 0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0, 0, 1));
    vb.EmplaceBack(XMFLOAT3( 0.0f,-1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0, 0, 1));
    for (size_t i = 0; i < vb.Size(); ++i)
    {
        PrintDebugMessage(L"Vertex[%d]: ", (int)i);
        auto& Pos = vb[i].Attrtribute<VertexFactory::Position3D>();
        PrintDebugMessage(L"Position: [ %f, %f, %f ]", Pos.x, Pos.y, Pos.z);
        auto& TexCoord = vb[i].Attrtribute<VertexFactory::TextureCoord>();
        PrintDebugMessage(L"Texcoord: [ %f, %f ]", TexCoord.x, TexCoord.y);
        auto& n = vb[i].Attrtribute<VertexFactory::Normal>();
        PrintDebugMessage(L"Normal: [ %f, %f, %f ]", n.x, n.y, n.z); 

        Pos.x += g_Timer.DeltaTime();
    }
}
