#pragma once

struct ShaderConstructDesc
{
	std::wstring ShaderFile;
	std::string  ShaderEntry;
	EShaderType  ShaderType;
};

class RHIShader
{
public:
	virtual ~RHIShader() {}

public:
	void SetShaderDesc(ShaderConstructDesc& Desc);

	std::wstring GetShaderFile() const;
	std::string GetShaderEntry() const;
	EShaderType GetShaderType() const;

protected:
	ShaderConstructDesc      ShaderDesc;
};