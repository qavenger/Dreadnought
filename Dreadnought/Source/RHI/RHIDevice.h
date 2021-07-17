#pragma once

#include "Texture.h"
#include "RenderTarget.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "PipelineStateObject.h"

//Clear Color/Depht/Stencil Value
struct ClearCDSValue
{
	float               ClearColorR;
	float               ClearColorG;
	float               ClearColorB;
	float               ClearColorA;
	bool                ClearValid;

	ClearCDSValue()
		:ClearColorR(0.f)
		, ClearColorG(0.f)
		, ClearColorB(0.f)
		, ClearColorA(1.f)
		, ClearValid(true)
	{

	}

	bool IsClearColorValid() const { return ClearValid; }
};

struct RHIRenderPassInfo
{
	bool           UseBackBuffer;
	RHIRenderTarget* SceneColor[MAX_RENDER_TARGET];
	ClearCDSValue  ClearColor[MAX_RENDER_TARGET];
	uint32         NumRenderTarget;
	RHIRenderTarget* SceneDepthZ;
	float          ClearDepth;
	uint8          ClearStencil;
	bool           ClearDepthValid;
	bool           ClearStencilValid;

	RHIRenderPassInfo()
		: UseBackBuffer(true)
		, NumRenderTarget(1)
		, ClearDepth(1.f)
		, ClearStencil(0)
		, ClearDepthValid(true)
		, ClearStencilValid(false)
	{
		for (uint32 Index = 0; Index < MAX_RENDER_TARGET; ++Index)
		{
			SceneColor[Index] = nullptr;
		}

		SceneDepthZ = nullptr;
	}

	bool IsClearDepthValid() const { return ClearDepthValid; }
	bool IsClearStencilValid() const { return ClearStencilValid; }
};

struct DrawInfo
{
	RHIIndexBuffer* IndexBuffer;
	RHIVertexBuffer* VertexBuffer;
	EPrimitiveTopology   PrimitiveTopology;
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

	virtual void BeginScopeEvent(const char* EventName) = 0;
	virtual void EndScopeEvent() = 0;
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void BeginRenderPass(const RHIRenderPassInfo& RPInfo) = 0;
	virtual void EndRenderPass() = 0;
	virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) const = 0;
	virtual void SetScissor(float Left, float Top, float Right, float Bottom) const = 0;
	virtual void ResetCommandList() = 0;
	virtual void FlushCommandQueue() = 0;
	virtual void FlushCommandQueueSync() = 0;
	virtual void WaitForGPU() = 0;
	virtual void Present() const = 0;
	virtual void Transition(void* Resource, EResourceState Before, EResourceState After) const = 0;

	//Create Resource Function
	virtual void CreateTextureXD(TextureDesc& Desc, RHITexture* Tex) = 0;
	virtual RHIIndexBuffer* CreateIndexBuffer() = 0;
	virtual RHIVertexBuffer* CreateVertexBuffer() = 0;
	virtual RHIShader* CreateShader() = 0;
	virtual RHIPipelineStateObject* CreatePipelineStateObject() = 0;


	virtual void BuildIndexBuffer(RHIIndexBuffer* IndexBuffer) = 0;
	virtual void BuildVertexBuffer(RHIVertexBuffer* VertexBuffer) = 0;
	virtual void BuildShader(RHIShader* Shader) = 0;
	virtual void BuildPipelineStateObject(RHIPipelineStateObject* PSO) = 0;


	virtual void SetPipelineStateObject(RHIPipelineStateObject* PSO) = 0;

	virtual void DrawElements(const DrawInfo& Info) = 0;

protected:
	HWND                WindowHandle;
	uint32              WindowWidth;
	uint32              WindowHeight;
	ETextureFormat      BackBufferFormat;
	ETextureFormat      DepthStencilFormat;
	uint32              SwapChainBufferCount;
	uint32              CurrentBackBufferIndex;
};