#pragma once

#define ETYPE\
	X(Position2			,	DirectX::XMFLOAT2	, DXGI_FORMAT_R32G32_FLOAT		, Position	, 2					, 0			) \
	X(Position3			,	DirectX::XMFLOAT3	, DXGI_FORMAT_R32G32B32_FLOAT	, Position	, 3					, 0			) \
	X(TextureCoord		,	DirectX::XMFLOAT2	, DXGI_FORMAT_R32G32_FLOAT		, Texcoord	, 2					, 0			) \
	X(TextureCoord2		,	DirectX::XMFLOAT4	, DXGI_FORMAT_R32G32B32A32_FLOAT, Texcoord	, 4					, 0			) \
	X(Normal			,	DirectX::XMFLOAT3	, DXGI_FORMAT_R32G32B32_FLOAT	, Normal	, 3					, 0			) \
	X(Tangent			,	DirectX::XMFLOAT3	, DXGI_FORMAT_R32G32B32_FLOAT	, Tangent	, 3					, 0			) \
	X(FloatColor3		,	DirectX::XMFLOAT3	, DXGI_FORMAT_R32G32B32_FLOAT	, Color		, 3					, 0			) \
	X(Color				,	::BGRAColor			, DXGI_FORMAT_R8G8B8A8_UNORM	, Color		, 4					, 0			) \
	X(FloatColor4		,	DirectX::XMFLOAT4	, DXGI_FORMAT_R32G32B32A32_FLOAT, Color		, 4					, 0			) \
	X(Count				,	DirectX::XMFLOAT4	, DXGI_FORMAT_UNKNOWN			, Unknown	, 0					, 0			)

enum class EElementType
{
#define X(a, b, c, d, e, f) a,
	ETYPE
#undef X
};

static int GetElementTypeIDByName(std::string name, int numComps)
{
	if (name == "Unknown") return -1;
#define X(a, b, c, d, e, f) else if( (name == #d) && (numComps == e) ) return (int)EElementType::a;
	ETYPE
#undef X
	return -1;
}

template<EElementType> struct Map;
#define X(a, b, c, d, e, f)\
template<> struct Map<EElementType::a>{\
using SysType = b;\
static constexpr DXGI_FORMAT Format = c;\
static constexpr const char* Semantic = #d;\
static constexpr const uint32 Type = (uint32)(EElementType::a);};
ETYPE
#undef X

class VertexLayout
{
public:
	class Element
	{
	public:
		Element(EElementType type, size_t offset, uint32 semanticIndex = 0);
		size_t Size()const NOENDBG;
		size_t GetOffset()const NOENDBG;
		size_t GetNextOffset()const NOENDBG;
		static constexpr size_t SizeOf(EElementType type)
		{
			using namespace DirectX;
			switch (type)
			{
			#define X(a, b, c, d, e, f)\
			case EElementType::a : return sizeof(Map<EElementType::a>::SysType);
				ETYPE
			#undef X
			}
			ThrowIfFalse(false, "Invalid element type");
			return 0;
		}
		EElementType GetType()const noexcept { return Type; }

		D3D12_INPUT_ELEMENT_DESC GetDesc(uint32 inputSlot) const
		{
			switch (Type)
			{
			#define X(a, b, c, d, e, f) \
			case EElementType::a: return GenerateDesc<EElementType::a>(GetOffset(), SemanticIndex, inputSlot);
				ETYPE
			#undef X
			}
			ThrowIfFalse(false, "Invalid element type");
			return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		}
	private:
		template<EElementType T>
		static constexpr D3D12_INPUT_ELEMENT_DESC GenerateDesc(size_t offset, uint32 semanticIndex = 0, uint32 inputSlot = 0) NOENDBG
		{
			return {
				Map<T>::Semantic,
				semanticIndex,
				Map<T>::Format,
				inputSlot,
				(uint32)offset,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				0
			};
		}
	private:
		EElementType Type;
		size_t Offset;
		uint32 SemanticIndex;
	};
	VertexLayout(uint32 slot = 0) : InputSlot(slot) {}
public:
	template<EElementType T>
	const Element& Resolve() const NOENDBG
	{
		for (auto& e : Elements)
		{
			if (e.GetType() == T)
			{
				return e;
			}
		}
		ThrowIfFalse(false, "Couldn't find element type");
		return Elements.front();
	}

	const Element GetElement(size_t index)const NOENDBG
	{
		assert(index >= 0 && index < Elements.size());
		return Elements[index];
	}

	VertexLayout& AppendElement(EElementType type, uint32 semanticIndex = 0) NOENDBG
	{
		Elements.emplace_back(type, Size(), semanticIndex);
		return *this;
	}

	size_t Size()const NOENDBG
	{
		return Elements.empty() ? 0 : Elements.back().GetNextOffset();
	}

	size_t ElementCount()const NOENDBG
	{
		return Elements.size();
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> GetD3DInputlayout()const NOENDBG
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> descs;
		descs.reserve(ElementCount());
		for (const auto& e : Elements)
		{
			descs.push_back(e.GetDesc(InputSlot));
		}
		return descs;
	}
private:
	uint32 InputSlot;
	std::vector<Element> Elements;
};

class Vertex
{
	friend class VertexBuffer;
public:
	template<EElementType T>
	auto& Attribute() NOENDBG
	{
		using namespace DirectX;
		const auto& element = Layout.Resolve<T>();
		auto pAttribute = pData + element.GetOffset();
		return *reinterpret_cast<typename Map<T>::SysType*>(pAttribute);
	}

	template<typename T>
	void SetAttributeByIndex(size_t index, T&& value) NOENDBG
	{
		using namespace DirectX;
		const auto& element = Layout.GetElement(index);
		auto pAttribute = pData + element.GetOffset();
		switch (element.GetType())
		{
#define X(a,b,c,d, e,f)\
			case EElementType::a: SetAttribute<EElementType::a>(pAttribute, std::forward<T>(value));break;
			ETYPE
#undef X
		default:
			ThrowIfFalse(false, "Invalid element type");
		}
	}
private:
	template<typename T, typename... Rest>
	void SetAttributeByIndex(size_t index, T&& first, Rest&&... rest)
	{
		SetAttributeByIndex(index, std::forward<T>(first));
		SetAttributeByIndex(index - 1, std::forward<Rest>(rest)...);
	}

	template<EElementType DestType, typename SrcType>
	void SetAttribute(void* pAttribute, SrcType&& val) NOENDBG
	{
		using Dest = typename Map<DestType>::SysType;
		if constexpr (std::is_assignable<Dest, SrcType>::Value)
		{
			*reinterpret_cast<Dest*>(pAttribute) = val;
		}
		else
		{
			ThrowIfFalse(false, "Parameter attribute type mismatch");
		}
	}
protected:
	Vertex(void* pData, const VertexLayout& layout)NOENDBG
		:
		pData(pData),
		Layout(layout)
	{
		assert(pData);
	}
private:
	void* pData = nullptr;
	const VertexLayout& Layout;
};

class ConstVertex
{
public:
	ConstVertex(const Vertex& v)NOENDBG
		: vertex(v)
	{}
	template<EElementType T>
	const auto& Attribute()const NOENDBG
	{
		return  const_cast<Vertex&>(vertex).Attribute<T>();
	}
private:
	Vertex vertex;
};

class VertexBuffer
{
public:
	VertexBuffer(VertexLayout layout) NOENDBG
		: Layout(std::move(layout)){}
	const VertexLayout& GetLayout()const noexcept { return Layout; }
	size_t NumVertices()const NOENDBG { return Data.size() / Layout.Size(); }
	size_t SizeInByte()const NOENDBG { return Data.size(); }

	template<typename... Params>
	void EmplaceBack(Params&&... params)
	{
		ThrowIfFalse(sizeof...(params) == Layout.ElementCount(), "Params layout does not match with the vertex layout");
		Data.resize(Data.size() + Layout.Size());
		
	}

	Vertex Back() NOENDBG
	{
		assert(!Data.empty());
		return Vertex(Data.data() + Data.size() - Layout.Size(), Layout);
	}

	Vertex Front() NOENDBG
	{
		assert(!Data.empty());
		return Vertex(Data.data(), Layout);
	}

	Vertex operator[](size_t index)NOENDBG
	{
		assert(!Data.empty());
		return Vertex{ Data.data() + Layout.Size() * index, Layout };
	}

	ConstVertex Back()const NOENDBG
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}

	ConstVertex Front()const NOENDBG
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}

	ConstVertex operator[](size_t index)const NOENDBG
	{
		return const_cast<VertexBuffer&>(*this)[index];
	}
private:
	std::vector<char> Data;
	VertexLayout Layout;
};

#undef ETYPE

class VertexFactory
{
public:
	/// <summary>
	/// Must be called before finalize the vertex layout
	/// </summary>
	/// <param name="layoutFlag">Element flags</param>
	/// <param name="output">output vertex layout</param>
	/// <returns>true if is newly created</returns>
	bool GetOrCreateCPUVertexLayout(uint32 layoutFlag, VertexLayout*& output);
	// must be called after creating cpu vertex layout
	const std::vector<D3D12_INPUT_ELEMENT_DESC>* Finalize(uint32 layoutFlag);
	const std::vector<D3D12_INPUT_ELEMENT_DESC>* GetGPUVertexLayout(uint32 layoutFlag)const;
private:
	std::vector<VertexLayout> CPUVertexLayoutArray;
	std::vector<std::vector<D3D12_INPUT_ELEMENT_DESC>> GPUVertexLayoutArray;
	std::map<uint32, size_t> VertexLayoutMap;
};