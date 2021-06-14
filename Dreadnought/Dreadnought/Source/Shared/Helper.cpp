#include "pch.h"
#include <Helper.h>

ExceptionBase::ExceptionBase(int line, const wchar_t* file) noexcept
	:
	Line(line), File(file)
{
}

const wchar_t* ExceptionBase::GetExceptionMessage() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const wchar_t* ExceptionBase::GetType() const noexcept
{
	return L"Exception base";
}

std::wstring ExceptionBase::GetOriginString() const noexcept
{
	std::wostringstream oss;
	oss << "[FILE]: " << File << std::endl
		<< "[LINE]: " << Line;
	return oss.str();
}

std::wstring HrException::HrToString(HRESULT hr) noexcept
{
	wchar_t* pMsgBuffer = nullptr;
	auto MsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMsgBuffer), 0, nullptr);
	if (MsgLen == 0) {
		return L"Unidentified error code";
	}

	std::wstring errorString = pMsgBuffer;
	LocalFree(pMsgBuffer);
	return errorString;
}

HrException::HrException(int line, const wchar_t* file, HRESULT hr)noexcept : ExceptionBase(line, file), Hr(hr) {}

const wchar_t* HrException::GetType() const noexcept { return L"HResult Exception"; }

const wchar_t* HrException::GetExceptionMessage() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl <<
		"[ERROR CODE]: " << Error() << std::endl <<
		"[Description]: " << GetErrorString().c_str() << std::endl <<
		GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

inline HRESULT HrException::Error() const noexcept { return Hr; }

std::wstring HrException::GetErrorString() const noexcept
{ 
	return HrToString(Hr); 
}

MsgException::MsgException(int line, const wchar_t* file, const wchar_t* msg) noexcept
	:
	ExceptionBase(line, file),
	Msg(msg)
{
}

const wchar_t* MsgException::GetType() const noexcept
{
	return L"Message Exception";
}

const wchar_t* MsgException::GetExceptionMessage() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl <<
		"[Description]: " << Msg << std::endl <<
		GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}
