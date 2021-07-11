#pragma once

#include "Texture.h"
#include "RenderTarget.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "PipelineStateObject.h"

struct RHIRenderPassInfo
{
	IRenderTarget SceneColor;
	IRenderTarget SceneDepthZ;
};

class IRHIDevice
{
public:
	virtual ~IRHIDevice() {}

	virtual void Init() = 0;
	virtual void Destroy() = 0;

	virtual void Resize(
		uint32 Width, 
		uint32 Height);

	virtual void CreateSwapChain(
		HWND Handle,
		uint32 Width,
		uint32 Height,
		uint32 BufferCount,
		ETextureFormat BBFormat,
		ETextureFormat DSFormat);

	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void BeginRenderPass(RHIRenderPassInfo& RPInfo) = 0;
	virtual void EndRenderPass() = 0;
	virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) const = 0;
	virtual void SetScissor(float Left, float Top, float Right, float Bottom) const = 0;
	virtual void FlushCommandQueue() = 0;
	virtual void FlushCommandQueueSync() = 0;
	virtual void WaitForGPU() = 0;
	virtual void Present() const = 0;

	//Create Resource Function
	virtual void CreateTextureXD(TextureDesc& Desc, ITexture* Tex) = 0;
	virtual IIndexBuffer* CreateIndexBuffer() = 0;
	virtual IVertexBuffer* CreateVertexBuffer() = 0;
	virtual IShader* CreateShader() = 0;


	virtual void SetIndexBuffer(IIndexBuffer* IndexBuffer) = 0;
	virtual void SetVertexBuffer(IVertexBuffer* VertexBuffer) = 0;
	virtual void SetShader(IShader* Shader) = 0;
	virtual void SetPipelineStateObject(IPipelineStateObject* PSO) = 0;

	virtual void DrawIndexedInstanced(IIndexBuffer* IndexBuffer, IVertexBuffer* VertexBuffer) = 0;

protected:
	HWND                WindowHandle;
	uint32              WindowWidth;
	uint32              WindowHeight;
	ETextureFormat      BackBufferFormat;
	ETextureFormat      DepthStencilFormat;
	uint32              SwapChainBufferCount;
	uint32              CurrentBackBufferIndex;
};