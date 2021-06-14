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

class ExceptionBase : public std::exception
{
public:
	ExceptionBase(int line, const wchar_t* file)noexcept;
	virtual const wchar_t* GetType()const noexcept;
	virtual const wchar_t* GetExceptionMessage()const noexcept;
	int GetLine()const noexcept { return Line; }
	const std::wstring& GetFile()const noexcept { return File; }
	std::wstring GetOriginString()const noexcept;
private:
	int Line;
	std::wstring File;
protected:
	mutable std::wstring whatBuffer;
};

class HrException : public ExceptionBase
{
public:
	static std::wstring HrToString(HRESULT hr)noexcept;

	HrException(int line, const wchar_t* file, HRESULT hr)noexcept;
	virtual const wchar_t* GetType()const noexcept override;
	virtual const wchar_t* GetExceptionMessage()const noexcept override;
	HRESULT Error()const noexcept;
	std::wstring GetErrorString()const noexcept;
private:
	HRESULT Hr;
};

class MsgException : public ExceptionBase
{
public:
	MsgException(int line, const wchar_t* file, const wchar_t* msg)noexcept;
	virtual const wchar_t* GetType()const noexcept override;
	virtual const wchar_t* GetExceptionMessage()const noexcept override;
private:
	std::wstring Msg;
};

#define SAFE_RELEASE(x) if(x) (x)->Release()

#define ThrowIfFailed(hr) if(FAILED(hr)) throw HrException(__LINE__, __FILEW__, hr)
#define ThrowIfFalse(value, msg) if(value == false) throw MsgException(__LINE__, __FILEW__, msg)
#define ThrowLastError(value) if(value == false) throw HrException(__LINE__, __FILEW__, GetLastError())


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

inline bool IsInsideRect(const RECT& in, const RECT& other)
{
	return in.left >= other.left && in.right <= other.right && in.top >= other.top && in.bottom <= other.bottom;
}