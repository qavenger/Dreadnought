#include "stdafx.h"
#include "Shader.h"


void RHIShader::SetShaderDesc(ShaderConstructDesc& Desc)
{
	ShaderDesc = Desc;
}

std::wstring RHIShader::GetShaderFile() const
{
	return ShaderDesc.ShaderFile;
}

std::string RHIShader::GetShaderEntry() const
{
	return ShaderDesc.ShaderEntry;
}

EShaderType RHIShader::GetShaderType() const
{
	return ShaderDesc.ShaderType;
}