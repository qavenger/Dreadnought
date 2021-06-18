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
	
#define ETYPE\
	X(Position2D		,	DirectX::XMFLOAT2	, DXGI_FORMAT_R32G32_FLOAT		, Position		) \
	X(Position3D		,	DirectX::XMFLOAT3	, DXGI_FORMAT_R32G32B32_FLOAT	, Position		) \
	X(TextureCoord		,	DirectX::XMFLOAT2	, DXGI_FORMAT_R32G32_FLOAT		, Texcoord		) \
	X(Normal			,	DirectX::XMFLOAT3	, DXGI_FORMAT_R32G32B32_FLOAT	, Normal		) \
	X(Tangent			,	DirectX::XMFLOAT3	, DXGI_FORMAT_R32G32B32_FLOAT	, Tangent		) \
	X(Float3Color		,	DirectX::XMFLOAT3	, DXGI_FORMAT_R32G32B32_FLOAT	, Color			) \
	X(Float4Color		,	DirectX::XMFLOAT4	, DXGI_FORMAT_R32G32B32A32_FLOAT, Color			) \
	X(Color				,	::BGRAColor			, DXGI_FORMAT_R8G8B8A8_UNORM	, Color			) \
	X(Count				,	DirectX::XMFLOAT4	, DXGI_FORMAT_UNKNOWN			, Unknown		)

	enum ElementType
	{
#define X(a, b, c, d) a,
		ETYPE
#undef X
	};

	template<ElementType> struct Map;
#define X(a, b, c, d)\
	template<> struct Map<a>{ \
	using SysType = b; \
	static constexpr DXGI_FORMAT Format = c; \
	static constexpr const char* Semantic = #d; \
	};
	ETYPE
#undef X

	class VertexLayout
	{
	public:
		class Element
		{
		public:
			Element(ElementType type, size_t offset)
				:Type(type), Offset(offset)
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
				#define X(a, b, c, d) \
				case a: return sizeof(Map<a>::SysType);
					ETYPE
				#undef X
				}
				ThrowIfFalse(false, L"Invalid Element Type");
				return 0;
			}

			ElementType GetType()const noexcept { return Type; }

			D3D12_INPUT_ELEMENT_DESC GetDesc() const noexcept(!_DEBUG)
			{
				switch (Type)
				{
				#define X(a, b, c, d) \
					case a: return GenerateDesc<a>(GetOffset()); 
				ETYPE
				#undef X
				}
				ThrowIfFalse(false, L"Invalid element type");
				return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			}

		private:
			template<ElementType T>
			static constexpr D3D12_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noexcept(!_DEBUG)
			{
				return { 
					Map<T>::Semantic,							// Semantic Name
					0,											// Semantic Index
					Map<T>::Format,								// DXGI_Format
					0,											// Input slot
					(uint)offset,								// ByteOffset
					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // InputSlotClass
					0											// InstanceDataStepRate 
				};
			}
		private:
			ElementType Type;
			size_t Offset;
		};

	public:
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

		VertexLayout& AppendElement(ElementType type)noexcept(!_DEBUG)
		{
			Elements.emplace_back(type, Size());
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
				desc.push_back(e.GetDesc());
			}
			return desc;
		}
	private:
		std::vector<Element> Elements;
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
			#define X(a,b,c,d)\
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