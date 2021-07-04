#include <stdafx.h>
#include "Error.h"

ExceptionBase::ExceptionBase(int32 line, const tchar* file, bool bIsFatal) noexcept
	:
	Line(line), File(file), bIsFatal(bIsFatal)
{
}

ExceptionBase::~ExceptionBase()
{
	if (bIsFatal)
	{
		PostQuitMessage(0);
	}
}

const tchar* ExceptionBase::GetType() const noexcept
{
	return "Exception Base";
}

const tchar* ExceptionBase::GetExceptionMessage() const noexcept
{
	std::ostringstream oss;
	oss << GetOriginString();
	ErrorMsgBuffer = oss.str();
	return ErrorMsgBuffer.c_str();
}

std::string ExceptionBase::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << (bIsFatal? "[FATAL ERROR]\n" : "[ERROR]")
		<< "[FILE]: " << File << std::endl
		<< "[LINE]: " << Line;
	return oss.str();
}

std::string HrException::HrToString(HRESULT hr) noexcept
{
	tchar* pMsgBuffer = nullptr;
	auto MsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuffer), 0, nullptr);
	if (MsgLen == 0) {
		return "Unidentified error code";
	}

	std::string errorString = pMsgBuffer;
	LocalFree(pMsgBuffer);
	return errorString;
}

HrException::HrException(int line, const tchar* file, HRESULT hr, bool bIsFatal) noexcept
	:
	ExceptionBase(line, file, bIsFatal),
	Hr(hr)
{
}

const tchar* HrException::GetType() const noexcept
{
	return "hResult Exception";
}

const tchar* HrException::GetExceptionMessage() const noexcept
{
	std::ostringstream oss;
	oss << GetOriginString() << std::endl
		<< "[ERROR CODE]: " << Error() << std::endl
		<< "[Description]: " << GetErrorString();
	ErrorMsgBuffer = oss.str();
	return ErrorMsgBuffer.c_str();
}

HRESULT HrException::Error() const noexcept
{
	return Hr;
}

std::string HrException::GetErrorString() const noexcept
{
	return HrToString(Hr);
}

MsgException::MsgException(int line, const tchar* file, const tchar* msg, bool bIsFatal) noexcept
	:
	ExceptionBase(line, file, bIsFatal),
	Msg(msg)
{
}

const tchar* MsgException::GetType() const noexcept
{
	return "Message Exception";
}

const tchar* MsgException::GetExceptionMessage() const noexcept
{
	std::ostringstream oss;
	oss << GetOriginString() << std::endl
		<< "[Description]: "<< Msg;
	ErrorMsgBuffer = oss.str();
	return ErrorMsgBuffer.c_str();
}
