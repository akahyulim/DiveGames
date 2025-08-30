#include "stdafx.h"
#include "ShaderProgram.h"
#include "Graphics.h"

namespace Dive
{
	ShaderProgram::ShaderProgram(std::shared_ptr<VertexShader> vs, std::shared_ptr<PixelShader> ps, Microsoft::WRL::ComPtr<ID3D11InputLayout> il,
		const std::string& name)
		: m_vs(vs)
		, m_ps(ps)
		, m_il(il)
		, m_name(name)
	{
	}

	void ShaderProgram::Bind()
	{
		Graphics::GetDeviceContext()->IASetInputLayout(m_il.Get());
		m_vs->Bind();
		m_ps->Bind();
	}
}