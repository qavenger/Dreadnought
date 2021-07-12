#pragma once

#include "../VertexBuffer.h"

class D3D12VertexBuffer : public IVertexBuffer
{
public:
	virtual void* GetVertexLayout();

public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

public:
	ComPtr<ID3D12Resource>                       VertexBufferGPU;
	ComPtr<ID3D12Resource>                       UploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW                     VertexBufferView;
	std::vector< D3D12_INPUT_ELEMENT_DESC>       VertexLayout;
};