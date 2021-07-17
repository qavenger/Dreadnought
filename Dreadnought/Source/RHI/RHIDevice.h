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
	IRenderTarget* SceneColor[MAX_RENDER_TARGET];
	ClearCDSValue  ClearColor[MAX_RENDER_TARGET];
	uint32         NumRenderTarget;
	IRenderTarget* SceneDepthZ;
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
	IIndexBuffer* IndexBuffer;
	IVertexBuffer* VertexBuffer;
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

	//Create Resource Function
	virtual void CreateTextureXD(TextureDesc& Desc, ITexture* Tex) = 0;
	virtual IIndexBuffer* CreateIndexBuffer() = 0;
	virtual IVertexBuffer* CreateVertexBuffer() = 0;
	virtual IShader* CreateShader() = 0;
	virtual IPipelineStateObject* CreatePipelineStateObject() = 0;


	virtual void BuildIndexBuffer(IIndexBuffer* IndexBuffer) = 0;
	virtual void BuildVertexBuffer(IVertexBuffer* VertexBuffer) = 0;
	virtual void BuildShader(IShader* Shader) = 0;
	virtual void BuildPipelineStateObject(IPipelineStateObject* PSO) = 0;


	virtual void SetPipelineStateObject(IPipelineStateObject* PSO) = 0;

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