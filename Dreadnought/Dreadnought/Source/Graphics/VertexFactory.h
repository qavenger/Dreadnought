#pragma once
namespace
{
	struct BGRAColor
	{
		uint8 A;
		uint8 R;
		uint8 G;
		uint8 B;
	};
}

namespace VertexFactory
{
	//Element Type			System Type			  Format						  Semantic    NumComponents		  SlotIndex
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

	enum ElementType
	{
#define X(a, b, c, d, e, f) a,
		ETYPE
#undef X
	};

	static int GetElementTypeIDByName(std::string name, int numComps)
	{
		if (name == "Unknown")	return -1;
#define X(a, b, c, d, e, f) else if( (name == #d) && (numComps == e) ) return (int)a;
		ETYPE
#undef X
			return -1;
	};

	template<ElementType> struct Map;
#define X(a, b, c, d, e, f)\
	template<> struct Map<a>{ \
	using SysType = b; \
	static constexpr DXGI_FORMAT Format = c; \
	static constexpr const char* Semantic = #d; \
	static constexpr const uint Type = (uint)a; \
	};
	ETYPE
#undef X

	class VertexLayout
	{
	public:
		class Element
		{
		public:
			Element(ElementType type, size_t offset, uint semanticIndex = 0)
				:Type(type), SemanticIndex(semanticIndex), Offset(offset)
			{}

			size_t Size()const noexcept(!_DEBUG)
			{
				return SizeOf(Type);
			}

			size_t GetOffset()const
			{
				return Offset;
			}

			size_t GetNextOffset()const
			{
				return Offset + Size();
			}

			static constexpr size_t SizeOf(ElementType type) noexcept(!_DEBUG)
			{
				using namespace DirectX;
				switch (type)
				{
				#define X(a, b, c, d, e, f) \
				case a: return sizeof(Map<a>::SysType);
					ETYPE
				#undef X
				}
				ThrowIfFalse(false, L"Invalid Element Type");
				return 0;
			}

			ElementType GetType()const noexcept { return Type; }

			D3D12_INPUT_ELEMENT_DESC GetDesc(uint InputSlot) const noexcept(!_DEBUG)
			{
				switch (Type)
				{
				#define X(a, b, c, d, e, f) \
					case a: return GenerateDesc<a>(GetOffset(), SemanticIndex, InputSlot); 
				ETYPE
				#undef X
				}
				ThrowIfFalse(false, L"Invalid element type");
				return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			}

		private:
			template<ElementType T>
			static constexpr D3D12_INPUT_ELEMENT_DESC GenerateDesc(size_t offset, uint semanticIndex = 0, uint InputSlot = 0) noexcept(!_DEBUG)
			{
				return { 
					Map<T>::Semantic,							// Semantic Name
					semanticIndex,								// Semantic Index
					Map<T>::Format,								// DXGI_Format
					InputSlot,									// Input slot
					(uint)offset,								// ByteOffset
					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // InputSlotClass
					0											// InstanceDataStepRate 
				};
			}
		private:
			ElementType Type;
			uint   SemanticIndex;
			size_t Offset;
		};

		VertexLayout() :InputSlot(0) {  }
		VertexLayout(uint slot):InputSlot(slot){}

	public:
		/// <summary>
		/// Get element type
		/// </summary>
		/// <returns></returns>
		template<ElementType Type>
		const Element& Resolve() const noexcept(!_DEBUG)
		{
			for (auto& e : Elements)
			{
				if (e.GetType() == Type)
				{
					return e;
				}
			}
			ThrowIfFalse(false, L"Could not resolve element type");
			return Elements.front();
		}

		const Element& GetElement(size_t Index)const noexcept(!_DEBUG)
		{
			assert(Index >= 0 && Index < Elements.size());
			return Elements[Index];
		}

		VertexLayout& AppendElement(ElementType type, uint semanticIndex = 0)noexcept(!_DEBUG)
		{
			Elements.emplace_back(type, Size(), semanticIndex);
			return *this;
		}

		size_t Size()const noexcept(!_DEBUG)
		{
			return Elements.empty() ? 0 : Elements.back().GetNextOffset();
		}

		size_t ElementCount()const noexcept(!_DEBUG)
		{
			return Elements.size();
		}

		std::vector<D3D12_INPUT_ELEMENT_DESC> GetD3DInputLayout()const noexcept(!_DEBUG)
		{
			std::vector<D3D12_INPUT_ELEMENT_DESC> desc;
			desc.reserve(ElementCount());
			for (const auto& e : Elements)
			{
				desc.push_back(e.GetDesc(InputSlot));
			}
			return desc;
		}
	private:
		std::vector<Element> Elements;
		uint InputSlot;
	};

	class Vertex
	{
		friend class VertexBuffer;
	public:
		template<ElementType Type>
		auto& Attrtribute() noexcept(!_DEBUG)
		{
			using namespace DirectX;
			const auto& element = Layout.Resolve<Type>();
			auto pAttribute = pData + element.GetOffset();
			return *reinterpret_cast<typename Map<Type>::SysType*>(pAttribute);
		}
		template<typename T>
		void SetAttributeByIndex(size_t index, T&& value)noexcept(!_DEBUG)
		{
			using namespace DirectX;
			const auto& element = Layout.GetElement(index);
			auto pAttribute = pData + element.GetOffset();
			switch (element.GetType())
			{
			#define X(a,b,c,d, e,f)\
				case a: SetAttribute<a>(pAttribute, std::forward<T>(value));break;
			ETYPE
			#undef X
			
			default:
				ThrowIfFalse(false, L"Invalid element type");
			}
		}
	private:
		template<typename T, typename... Rest>
		void SetAttributeByIndex(size_t index, T&& first, Rest&&... rest)
		{
			SetAttributeByIndex(index, std::forward<T>(first));
			SetAttributeByIndex(index + 1, std::forward<Rest>(rest)...);
		}

		template<ElementType DestLayoutType, typename SrcType>
		void SetAttribute(char* pAttribute, SrcType&& val) noexcept(!_DEBUG)
		{
			using Dest = typename Map<DestLayoutType>::SysType;
			if constexpr (std::is_assignable<Dest, SrcType>::value)
			{
				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else
			{
				ThrowIfFalse(false, L"Parameter attribute type mismatch");
			}
		}
	protected:
		Vertex(char* pData, const VertexLayout& layout)noexcept(!_DEBUG)
			:
			pData(pData), Layout(layout)
		{
			assert(pData);
		}
	private:
		char* pData = nullptr;
		const VertexLayout& Layout;
	};

	class ConstVertex
	{
	public:
		ConstVertex(const Vertex& v)noexcept(!_DEBUG)
			:vertex(v)
		{}
		template<ElementType Type>
		const auto& Attribute()const noexcept(!_DEBUG)
		{
			return const_cast<Vertex&>(vertex).Attrtribute<Type>();
		}
	private:
		Vertex vertex;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout) noexcept(!_DEBUG)
			: Layout(std::move(layout))
		{}
		const VertexLayout& GetVertexLayout()const noexcept
		{
			return Layout;
		}

		size_t Size()const noexcept(!_DEBUG)
		{
			return Data.size() / Layout.Size();
		}

		size_t SizeInBytes()const noexcept(!_DEBUG)
		{
			return Data.size();
		}

		template<typename... Params>
		void EmplaceBack(Params&&... params)
		{
			ThrowIfFalse(sizeof...(params) == Layout.ElementCount(), L"Params layout does not match with the vertex layout");
			Data.resize(Data.size() + Layout.Size());
			Back().SetAttributeByIndex(0, std::forward<Params>(params)...);
		}

		Vertex Back()noexcept(!_DEBUG)
		{
			assert(!Data.empty());
			return Vertex{ Data.data() + Data.size() - Layout.Size(),Layout };
		}

		Vertex Front()noexcept(!_DEBUG)
		{
			assert(!Data.empty());
			return Vertex{ Data.data(),Layout };
		}

		Vertex operator[](size_t index)noexcept(!_DEBUG)
		{
			assert(index < Size());
			return Vertex{ Data.data() + Layout.Size() * index , Layout };
		}

		ConstVertex Back() const noexcept(!_DEBUG)
		{
			return const_cast<VertexBuffer*>(this)->Back();
		}

		ConstVertex Front() const noexcept(!_DEBUG)
		{
			return const_cast<VertexBuffer*>(this)->Front();
		}

		ConstVertex operator[](size_t index) const noexcept(!_DEBUG)
		{
			return const_cast<VertexBuffer&>(*this)[index];
		}
	private:
		std::vector<char> Data;
		VertexLayout Layout;
	};


	class FVertexFactory : public INoncopyable
	{
	public:
		static FVertexFactory& GetInstance() { return VertexFactory; }
	public:
		/// <summary>
		/// Must be called before finalize the vertex layout
		/// </summary>
		/// <param name="layoutFlag">Element flags</param>
		/// <param name="output">output vertex layout</param>
		/// <returns>true if is newly created</returns>
		bool GetOrCreateCPUVertexLayout(uint layoutFlag, VertexLayout*& output);
		// must be called after creating cpu vertex layout
		const std::vector<D3D12_INPUT_ELEMENT_DESC>* Finalize(uint layoutFlag);
		const std::vector<D3D12_INPUT_ELEMENT_DESC>* GetGPUVertexLayout(uint layoutFlag)const;
		//std::vector<D3D12_INPUT_ELEMENT_DESC>* AddLayout(uint layoutFlag);
	private:
		static FVertexFactory VertexFactory;
		std::vector<VertexLayout> CPUVertexLayoutArray;
		std::vector<std::vector<D3D12_INPUT_ELEMENT_DESC>> GPUVertexLayoutArray;
		std::map<uint, size_t> VertexLayoutMap;
	};
}
/*
struct SimpleVertex
{
	float3 pos;
	float4 color;
};

struct Vertex
{
	float3 pos;
	float3 normal;
	float3 tangent;
	float2 uv;
};

template<int UVCount = 1>
struct VertexAddition
{
	float3 color;
	float4 uv1[UVCount];
};

struct VertexSkeletal
{
	int4 boneIndices;
	float4 boneWeight;
};

class VertexFactory
{
public:
	static D3D12_INPUT_ELEMENT_DESC* GetSkeletalMeshVertex();
};
*/