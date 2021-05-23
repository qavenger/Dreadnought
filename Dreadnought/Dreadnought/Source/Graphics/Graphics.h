#pragma once
class Window;
class Graphics
{
public:
	~Graphics();
public:
	static Graphics* GetInstance();
	bool Init(Window* Wnd);
	void PostInit();
public:
	static void ReportLiveObject();
	uint2 GetResolution(int index)const;
	uint GetNumResolution()const { return (uint)uResolutions.size(); }
	void Resize(int resolutionIndex, EWindowMode resizeMode = EWindowMode::WINDOW);
	void Resize(uint width, uint height, EWindowMode resizeMode = EWindowMode::WINDOW);
private:
	Graphics() {};
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
private:
	HRESULT LogAdapter();
	HRESULT CreateDeviceAndFence();
	HRESULT CreateCommandObjects();
	HRESULT CreateViewDepandentResource(uint Width, uint Height);
	void ReleaseViewDepandentResource();
	uint PrevResolution();
private:
	UINT DescriptorHandleIncrementSize[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};
	UINT BackBufferCount = 2;
	UINT SelectedAdapter = -1;
	uint SelectedResolution = -1;
private:
	ComPtr<IDXGIFactory7> pFactory;
	ComPtr<ID3D12Device6> pDevice;
	ComPtr<ID3D12Fence1> pFence;
	ComPtr<ID3D12CommandQueue> pRenderCmdQueue;
	ComPtr<ID3D12CommandAllocator> pDirectCmdListAlloc;
	ComPtr<ID3D12GraphicsCommandList6> pCommandList;
	ComPtr<IDXGISwapChain4> pSwapChain;
private:
	std::vector<IDXGIAdapter*> pAdapters;
	std::vector<IDXGIOutput*> pOutputs;
	std::vector<uint> uResolutions;
private:
	Window* pWindow = nullptr;
};

