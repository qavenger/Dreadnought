#include <stdafx.h>
#include "VertexFactory.h"

VertexLayout::Element::Element(EElementType type, size_t offset, uint32 semanticIndex)
	:Type(type), Offset(offset), SemanticIndex(semanticIndex)
{
}

size_t VertexLayout::Element::Size()const NOENDBG
{
	return SizeOf(Type);
}

size_t VertexLayout::Element::GetOffset()const NOENDBG
{
	return Offset;
}

size_t VertexLayout::Element::GetNextOffset()const NOENDBG
{
	return Size() + Offset;
}

bool VertexFactory::GetOrCreateCPUVertexLayout(uint32 layoutFlag, VertexLayout*& output)
{
	if (layoutFlag == 0)
	{
		output = nullptr;
		return false;
	}

	auto found = VertexLayoutMap.find(layoutFlag);
	if (found == VertexLayoutMap.end())
	{
		VertexLayoutMap[layoutFlag] = CPUVertexLayoutArray.size();
		CPUVertexLayoutArray.push_back(std::move(VertexLayout()));
		output = &CPUVertexLayoutArray.back();
		return true;
	}

	output = &CPUVertexLayoutArray[found->second];
	return false;
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>* VertexFactory::Finalize(uint32 layoutFlag)
{
	if (layoutFlag == 0) return nullptr;
	ThrowIfFalse(CPUVertexLayoutArray.size() - GPUVertexLayoutArray.size() == 1, 
		"GetOrCreateCPUVertexLayout() and Finalize() call count mismatched, make sure to call GetOrCreateCPUVertexLayout() before finalize the vertex factory"
	);
	GPUVertexLayoutArray.push_back(std::move(CPUVertexLayoutArray.back().GetD3DInputlayout()));
	return &GPUVertexLayoutArray.back();;
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>* VertexFactory::GetGPUVertexLayout(uint32 layoutFlag) const
{
	auto i = VertexLayoutMap.find(layoutFlag);
	if (i == VertexLayoutMap.end()) return nullptr;
	return &GPUVertexLayoutArray[i->second];
}
