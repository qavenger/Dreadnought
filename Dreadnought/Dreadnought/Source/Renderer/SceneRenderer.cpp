#include "pch.h"
#include "SceneRenderer.h"
#include "Timer.h"

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
}

void SceneRenderer::OnRender()
{
    float color[] = { sin(g_Timer.Elapsed()),0,0,1 };
    CommandList->ClearRenderTargetView(DeviceResources->GetRenderTargetView(),
        color,
        1,
        &WindowRect
    );
}
