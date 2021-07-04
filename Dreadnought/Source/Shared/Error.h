#pragma once

class ExceptionBase : public std::exception
{
public:
	ExceptionBase(int32 line, const tchar* file, bool bIsFatal)noexcept;
	virtual ~ExceptionBase();
	virtual const tchar* GetType()const noexcept;
	virtual const tchar* GetExceptionMessage()const noexcept;
	int GetLine()const noexcept { return Line; }
	const std::string& GetFile()const noexcept { return File; }
	bool IsFalalError()const noexcept { return bIsFatal; }
	std::string GetOriginString()const noexcept;
private:
	int32 Line;
	std::string File;
	bool bIsFatal;
protected:
	mutable std::string ErrorMsgBuffer;
};

class HrException : public ExceptionBase
{
	static std::string HrToString(HRESULT hr)noexcept;
public:
	HrException(int line, const tchar* file, HRESULT hr, bool bIsFatal = false)noexcept;
	virtual const tchar* GetType()const noexcept override;
	virtual const tchar* GetExceptionMessage()const noexcept override;
	HRESULT Error()const noexcept;
	std::string GetErrorString()const noexcept;
private:
	HRESULT Hr;
};

class MsgException : public ExceptionBase
{
public:
	MsgException(int line, const tchar* file, const tchar* msg, bool bIsFatal = false)noexcept;
	virtual const tchar* GetType()const noexcept override;
	virtual const tchar* GetExceptionMessage()const noexcept override;
private:
	std::string Msg;
};

#define ThrowIfFailed(hr) {HRESULT Hr = hr; if(FAILED(Hr)) throw HrException(__LINE__, __FILE__, Hr);}
#define ThrowIfFalse(v,msg) if(v == false) throw MsgException(__LINE__,__FILE__,msg)
#define ThrowLastError(v) if(v == false) throw HrException(__LINE__,__FILE__, GetLastError())
#define FatalIfFalse(v, msg) if(v == false) throw MsgException(__LINE__,__FILE__,msg, true)
#define FatalIfFailed(hr) {HRESULT Hr = hr; if(FAILED(Hr)) throw HrException(__LINE__, __FILE__, Hr, true);}
#define FatalAndThrowLastError(v) if(v == false) throw HrException(__LINE__,__FILE__, GetLastError(), true)