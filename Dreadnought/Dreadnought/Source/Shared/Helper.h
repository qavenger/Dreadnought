#pragma once

#if defined(DEBUG) || defined(_DEBUG)
	#define PrintDebugMessage(msg, ...) wprintf(msg, __VA_ARGS__);wprintf(L"\n")
	#define PrintDebugInt(msg) wprintf(L"%d\n", msg)
	#define PrintDebugFloat(msg) wprintf(L"%f\n", msg)
#else
	#define PrintDebugMessage(msg) 
	#define PrintDebugInt(msg)
	#define PrintDebugFloat(msg)
#endif

class HrException : public std::runtime_error
{
	inline std::string HrToString(HRESULT hr)
	{
		char str[64] = {};
		sprintf_s(str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return str;
	}
public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), Hr(hr) {}
	HRESULT Error()const { return Hr; }
private:
	HRESULT Hr;
};

#define SAFE_RELEASE(x) if(x) (x)->Release()

inline void ThrowIfFailed(HRESULT hr)
{
	if(FAILED(hr))
	{
		throw HrException(hr);
	}
}

inline void ThrowIfFailed(HRESULT hr, const wchar_t* msg)
{
	if (FAILED(hr))
	{
		OutputDebugString(msg);
		throw HrException(hr);
	}
}

inline void ThrowIfFalse(bool value)
{
	ThrowIfFailed(value ? S_OK : E_FAIL);
}

inline void ThrowIfFalse(bool value, const wchar_t* msg)
{
	ThrowIfFailed(value ? S_OK : E_FAIL, msg);
}

inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize)
{
	if (path == nullptr)
	{
		throw std::exception();
	}

	DWORD size = GetModuleFileName(nullptr, path, pathSize);
	if (size == 0 || size == pathSize)
	{
		// Method failed or path was truncated.
		throw std::exception();
	}

	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash)
	{
		*(lastSlash + 1) = L'\0';
	}
}

// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DEBUG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
	pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
	WCHAR fullName[50];
	if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
	{
		pObject->SetName(fullName);
	}
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

inline UINT Align(UINT size, UINT alignment)
{
	return (size + (alignment - 1)) & ~(alignment - 1);
}

inline UINT CalculateConstantBufferByteSize(UINT byteSize)
{
	// Constant buffer size is required to be aligned.
	return Align(byteSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
}

