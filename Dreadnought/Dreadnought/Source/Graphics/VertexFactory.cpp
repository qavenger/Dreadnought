#include "pch.h"
#include "VertexFactory.h"

D3D12_INPUT_ELEMENT_DESC* VertexFactory::GetSkeletalMeshVertex()
{
	D3D12_INPUT_ELEMENT_DESC element;
	element.SemanticName = "POSITION";
	element.SemanticIndex = 0;
	element.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	element.InputSlot = 0;
	element.AlignedByteOffset = 0;
	element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	element.InstanceDataStepRate = 0;
	return nullptr;
}
