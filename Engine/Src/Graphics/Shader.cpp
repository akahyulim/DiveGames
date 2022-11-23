#include "divepch.h"
#include "Shader.h"
#include "Graphics.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	Shader::Shader(Context* pContext, eShaderType type)
		: Object(pContext),
		m_pGraphics(GetSubsystem<Graphics>()),
		m_Type(type)
	{
		DV_ASSERT(m_pGraphics->IsInitialized());
	}

	Shader::~Shader()
	{
		DV_LOG_ENGINE_TRACE("Shader �Ҹ��� ȣ��");
	}

	bool Shader::Compile(const std::string& filepath)
	{
		if (!m_pGraphics || !m_pGraphics->IsInitialized())
			return false;

		if (!std::filesystem::exists(filepath))
		{
			DV_LOG_ENGINE_ERROR("Shader::Compile - ���޹��� ����({:s})�� �������� �ʽ��ϴ�.", filepath);
			return false;
		}

		// ���, �̸� ������ ������ ����. => ���ҽ� �ʿ� ����?
		// �̸��� ��� ��ũ�α��� �߰��Ͽ� �����ϸ� ������ �ϴ� �о�.

		// �÷��׸� �� �� �ٵ��� �� ��?
		UINT flags = 0;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_PREFER_FLOW_CONTROL;// | D3DCOMPILE_ENABLE_STRICTNESS;
#else
		flags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		// defines�� ' '�� �����ؼ� vector�� �ְ�
		// �ٽ� �� vector�� ���� '1'�� macro�� �߰��Ѵ�.
		// urho�� ��� d3d11�� �߰� macro�� �����ϱ⵵ �ߴ�.
		// �ϴ��� sparatan �������� ����...
		std::vector<D3D_SHADER_MACRO> macros =
		{
			D3D_SHADER_MACRO{"COMPILEVS", m_Type == eShaderType::Vertex ? "1" : "0"},
			D3D_SHADER_MACRO{"COMPILEPS", m_Type == eShaderType::Pixel ? "1" : "0"}
		};
		// ��ũ�θ� �̸� ������ �� �������Ѵٸ�
		// �ᱹ �ϳ��� ���Ϸ� ���� ������ ���̴��� ����� �� �� �ۿ� ����.
		// �׷��� �̸��� ��ũ�α��� �߰��� ���ΰ�?
		for (const auto& macro : m_Defines)
		{
			macros.emplace_back(D3D_SHADER_MACRO{ macro.first.c_str(), macro.second.c_str() });
		}
		// �������� ������ nullptr, nullptr�̾�� �ϳ� ����.
		macros.emplace_back(D3D_SHADER_MACRO{ nullptr, nullptr });

		const char* pEntryPoint = nullptr;
		const char* pTarget = nullptr;
		if (m_Type == eShaderType::Vertex)
		{
			pEntryPoint = "mainVS";
			pTarget = "vs_5_0";
		}
		else
		{
			pEntryPoint = "mainPS";
			pTarget = "ps_5_0";
		}

		std::wstring fileName;
		fileName.assign(filepath.begin(), filepath.end());

		ID3D10Blob* pErrorMsg = nullptr;

		if (FAILED(D3DCompileFromFile(
			fileName.c_str(),
			macros.data(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			pEntryPoint,
			pTarget,
			flags,
			0,
			&m_pShaderBuffer,
			&pErrorMsg
		)))
		{
			if (pErrorMsg)
			{
				DV_LOG_ENGINE_ERROR("Shader::Compile - Shader Compile ����: {:s}", (char*)pErrorMsg->GetBufferPointer());
				DV_RELEASE(pErrorMsg);
			}
			return false;
		}
		DV_RELEASE(pErrorMsg);
			
		// ��ü �̸��� �ٿ��� ����ϱ�.
		if (m_Type == eShaderType::Vertex)
			DV_LOG_ENGINE_DEBUG("Shader::Compile - ���� ���̴��� ������ �Ͽ����ϴ�.");
		else
			DV_LOG_ENGINE_DEBUG("Shader::Compile - �ȼ� ���̴��� ������ �Ͽ����ϴ�.");

		parseParameters();
		calculateConstantBufferSizes();

		if (m_Type == eShaderType::Vertex)
		{
			if (FAILED(m_pGraphics->GetDevice()->CreateVertexShader(
				m_pShaderBuffer->GetBufferPointer(),
				m_pShaderBuffer->GetBufferSize(),
				nullptr,
				m_pVertexShader.ReleaseAndGetAddressOf())))
			{
				DV_LOG_ENGINE_ERROR("Shader::Compile - ���� ���̴�({:s}) ������ �����Ͽ����ϴ�.", m_Name);
				return false;
			}

			DV_LOG_ENGINE_DEBUG("Shader::Compile - ���� ���̴��� �����Ͽ����ϴ�.");
		}
		else
		{
			if (FAILED(m_pGraphics->GetDevice()->CreatePixelShader(
				m_pShaderBuffer->GetBufferPointer(),
				m_pShaderBuffer->GetBufferSize(),
				NULL,
				m_pPixelShader.ReleaseAndGetAddressOf())))
			{
				DV_LOG_ENGINE_ERROR("Shader::Compile - �ȼ� ���̴�({:s}) ������ �����Ͽ����ϴ�.", m_Name);
				return false;
			}

			DV_LOG_ENGINE_DEBUG("Shader::Compile - �ȼ� ���̴��� �����Ͽ����ϴ�.");
		}

		return true;
	}

	void* Shader::GetShader()
	{
		return m_Type == eShaderType::Vertex ?
			static_cast<void*>(m_pVertexShader.Get()) : static_cast<void*>(m_pPixelShader.Get());
	}

	bool Shader::HasParameter(const std::string& name) const
	{
		if (!m_Parameters.empty())
		{
			auto it = m_Parameters.find(name);
			return it != m_Parameters.end();
		}

		return false;
	}

	// �ڵ� ���ϼ��� ���� ������, ���̴� �κе� ������ �Լ��� ������ ���� ���� �� ����.
	// �׸��� �� �Լ��� public���� �ٲ���.
	// �׷��� �ᱹ �̵��� ���� ȣ���ϴ� �Լ��� �� �ʿ�������.
	void Shader::parseParameters()
	{
		ID3D11ShaderReflection* pReflection = nullptr;
		if (FAILED(D3DReflect(m_pShaderBuffer->GetBufferPointer(), m_pShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection)))
		{
			return;
		}

		D3D11_SHADER_DESC shaderDesc;
		pReflection->GetDesc(&shaderDesc);

		// vs�� �Է� �Ķ����?�� �ؽø� ����� ��

		std::map<std::string, unsigned int> cbRegisterMap;

		for (unsigned int i = 0; i < shaderDesc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC desc;
			pReflection->GetResourceBindingDesc(i, &desc);
			std::string resourceName(desc.Name);
			
			if (desc.Type == D3D_SIT_CBUFFER)
				cbRegisterMap[resourceName] = desc.BindPoint;
			else if (desc.Type == D3D_SIT_SAMPLER && desc.BindPoint < 16)
				m_bUseTextureUnits[desc.BindPoint] = true;
		}

		for (unsigned int i = 0; i < shaderDesc.ConstantBuffers; ++i)
		{
			auto pConstantBuffer = pReflection->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC desc;
			pConstantBuffer->GetDesc(&desc);

			unsigned int cbRegister = cbRegisterMap[std::string(desc.Name)];

			for (unsigned int j = 0; j < desc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* pVar = pConstantBuffer->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC varDesc;
				pVar->GetDesc(&varDesc);

				std::string varName(varDesc.Name);
				// �̰� urho�� ������ ���̹� ��å�� �� �ϴ�.
				//if (varName[0] == 'c')
				{
				//	varName = varName.substr(1);
					m_Parameters[varName] = ShaderParameter(m_Type, varName, varDesc.StartOffset, varDesc.Size, cbRegister);
				}
			}
		}

		DV_RELEASE(pReflection);
	}

	// constant buffer�� ���� ũ�⸦ ����Ѵ�.
	// ���� ��ü�� �� 7������ ���� �� �ִ�.
	void Shader::calculateConstantBufferSizes()
	{
		for (unsigned int i = 0; i < 7; ++i)
			m_ConstantBufferSizes[i] = 0;

		// parameter�� ������� ����� ���� �ƴϱ⿡
		// �̷��� ����ؾ� ���� ũ�Ⱑ ���´�.
		auto it = m_Parameters.begin();
		for (it; it != m_Parameters.end(); ++it)
		{
			if (it->second.m_Index < 7)
			{
				unsigned int oldSize = m_ConstantBufferSizes[it->second.m_Index];
				unsigned int paramEnd = it->second.m_Offset + it->second.m_Size;
				if (paramEnd > oldSize)
					m_ConstantBufferSizes[it->second.m_Index] = paramEnd;
			}
		}
	}
}