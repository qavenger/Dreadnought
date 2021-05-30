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
	enum ElementType
	{
		Position2D,
		Position3D,
		TextureCoord,
		Normal,
		Tangent,
		Float3Color,
		Float4Color,
		Color,
		Count,
	};

	template<ElementType> struct Map;
	template<> struct Map<Position2D>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32_FLOAT;
		static constexpr const char* Semantic = "Position";
	};
	template<> struct Map<Position3D>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* Semantic = "Position";
	};
	template<> struct Map<TextureCoord>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32_FLOAT;
		static constexpr const char* Semantic = "Texcoord";
	};
	template<> struct Map<Normal>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* Semantic = "Normal";
	};
	template<> struct Map<Tangent>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* Semantic = "Tangent";
	};
	template<> struct Map<Float3Color>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* Semantic = "Color";
	};
	template<> struct Map<Float4Color>
	{
		using SysType = DirectX::XMFLOAT4;
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		static constexpr const char* Semantic = "Color";
	};
	template<> struct Map<Color>
	{
		using SysType = ::BGRAColor;
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		static constexpr const char* Semantic = "Color";
	};

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
				case Position2D:
					return sizeof(Map<Position2D>::SysType);
				case TextureCoord:
					return sizeof(Map<TextureCoord>::SysType);
				case Position3D:
					return sizeof(Map<Position3D>::SysType);
				case Normal:
					return sizeof(Map<Normal>::SysType);
				case Tangent:
					return sizeof(Map<Tangent>::SysType);
				case Float3Color:
					return sizeof(Map<Float3Color>::SysType);
				case Float4Color:
					return sizeof(Map<Float4Color>::SysType);
				case Color:
					return sizeof(Map<Color>::SysType);
				}
				ThrowIfFalse(false, L"Invalid Element Type");
				return 0;
			}

			ElementType GetType()const noexcept { return Type; }

			D3D12_INPUT_ELEMENT_DESC GetDesc() const noexcept(!_DEBUG)
			{
				switch (Type)
				{
				case Position2D:
					return GenerateDesc<Position2D>(GetOffset());
				case Position3D:
					return GenerateDesc<Position3D>(GetOffset());
				case TextureCoord:
					return GenerateDesc<TextureCoord>(GetOffset());
				case Normal:
					return GenerateDesc<Normal>(GetOffset());
				case Tangent:
					return GenerateDesc<Tangent>(GetOffset());
				case Float3Color:
					return GenerateDesc<Float3Color>(GetOffset());
				case Float4Color:
					return GenerateDesc<Float4Color>(GetOffset());
				case Color:
					return GenerateDesc<Color>(GetOffset());
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
			return *reinterpret_cast<Map<Type>::SysType*>(pAttribute);
		}
		template<typename T>
		void SetAttributeByIndex(size_t index, T&& value)noexcept(!_DEBUG)
		{
			using namespace DirectX;
			const auto& element = Layout.GetElement(index);
			auto pAttribute = pData + element.GetOffset();
			switch (element.GetType())
			{
			case Position2D:
				SetAttribute<Position2D>(pAttribute, std::forward<T>(value));
				break;
			case TextureCoord:
				SetAttribute<TextureCoord>(pAttribute, std::forward<T>(value));
				break;
			case Position3D:
				SetAttribute<Position3D>(pAttribute, std::forward<T>(value));
				break;
			case Normal:
				SetAttribute<Normal>(pAttribute, std::forward<T>(value));
				break;
			case Tangent:
				SetAttribute<Tangent>(pAttribute, std::forward<T>(value));
				break;
			case Float3Color:
				SetAttribute<Float3Color>(pAttribute, std::forward<T>(value));
				break;
			case Float4Color:
				SetAttribute<Float4Color>(pAttribute, std::forward<T>(value));
				break;
			case Color:
				SetAttribute<Color>(pAttribute, std::forward<T>(value));
				break;
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