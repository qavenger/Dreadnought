#include "stdafx.h"
#include "Shader.h"


void IShader::SetShaderDesc(ShaderConstructDesc& Desc)
{
	ShaderDesc = Desc;
}

std::wstring IShader::GetShaderFile() const
{
	return ShaderDesc.ShaderFile;
}

std::string IShader::GetShaderEntry() const
{
	return ShaderDesc.ShaderEntry;
}

EShaderType IShader::GetShaderType() const
{
	return ShaderDesc.ShaderType;
}