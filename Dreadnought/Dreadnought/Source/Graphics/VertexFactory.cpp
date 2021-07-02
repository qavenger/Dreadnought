#include "pch.h"
#include "VertexFactory.h"

using namespace VertexFactory;

FVertexFactory& FVertexFactory::GetInstance()
{
	static FVertexFactory instance;
	return instance;
}

bool FVertexFactory::GetOrCreateCPUVertexLayout(uint layoutFlag, VertexLayout*& output)
{
	if (layoutFlag == 0)
	{
		output = nullptr;
		return false;
	}
	if (VertexLayoutMap.find(layoutFlag) == VertexLayoutMap.end())
	{
		VertexLayoutMap[layoutFlag] = CPUVertexLayoutArray.size();
		CPUVertexLayoutArray.push_back(std::move(VertexLayout()));
		output = &CPUVertexLayoutArray.back();
		return true;
	}

	output = &CPUVertexLayoutArray[VertexLayoutMap[layoutFlag]];
	return false;
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>* FVertexFactory::Finalize(uint layoutFlag)
{
	if (layoutFlag == 0) return nullptr;

	ThrowIfFalse(CPUVertexLayoutArray.size() - GPUVertexLayoutArray.size() == 1, L"Vertex Layout GetOrCreate() and Finalize() call number mismatched");
	
	GPUVertexLayoutArray.push_back(std::move(CPUVertexLayoutArray.back().GetD3DInputLayout()));

	return &GPUVertexLayoutArray.back();
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>* FVertexFactory::GetGPUVertexLayout(uint layoutFlag) const
{
	auto i = VertexLayoutMap.find(layoutFlag);
	if (i == VertexLayoutMap.end())return nullptr;
	return &GPUVertexLayoutArray[VertexLayoutMap.at(layoutFlag)];
}

//std::vector<D3D12_INPUT_ELEMENT_DESC>* FVertexFactory::AddLayout(uint layoutFlag)
//{
//	if (layoutFlag == 0) return nullptr;
//	// not found
//	if (VertexLayoutMap.find(layoutFlag) == VertexLayoutMap.end())
//	{
//		VertexLayout vertexLayout;
//		uint flag = layoutFlag;
//		for (int i = 0; flag != 0 && i < ElementType::Count; ++i, flag >>= 1)
//		{
//			if (flag & 1)
//			{
//				vertexLayout.AppendElement((ElementType(i)));
//			}
//		}
//		if (vertexLayout.ElementCount() > 0)
//		{
//			GPUVertexLayoutArray.push_back(vertexLayout.GetD3DInputLayout());
//			CPUVertexLayoutArray.push_back(std::move(vertexLayout));
//			VertexLayoutMap[layoutFlag] = CPUVertexLayoutArray.size() - 1;
//			return &GPUVertexLayoutArray.back();
//		}
//	}
//	
//	return &GPUVertexLayoutArray[ VertexLayoutMap[layoutFlag]];
//}
