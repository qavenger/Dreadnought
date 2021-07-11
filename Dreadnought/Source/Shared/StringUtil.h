#pragma once
#define INITIAL_BUFFER_SIZE 512

static std::string FormatString(const tchar* fmt, ...) noexcept
{
	int32 bufferSize = INITIAL_BUFFER_SIZE;
	tchar buffer[INITIAL_BUFFER_SIZE];
	int32 result = -1;



	va_list ap;
	va_start(ap, fmt);
	
	result = vsnprintf(buffer, bufferSize, fmt, ap);
	va_end(ap);
	if (result < bufferSize)
	{
		return buffer;
	}

	std::string rs(result + 1, 0);
	va_start(ap, fmt);
	result = vsnprintf_s(rs.data(), result+1, result + 1, fmt, ap);
	va_end(ap);
	return rs;
}