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
		DV_LOG_ENGINE_TRACE("Shader 소멸자 호출");
	}

	bool Shader::Compile(const std::string& filepath)
	{
		if (!m_pGraphics || !m_pGraphics->IsInitialized())
			return false;

		if (!std::filesystem::exists(filepath))
		{
			DV_LOG_ENGINE_ERROR("Shader::Compile - 전달받은 파일({:s})이 존재하지 않습니다.", filepath);
			return false;
		}

		// 경로, 이름 등으로 나누어 저장. => 리소스 쪽에 구현?
		// 이름의 경우 매크로까지 추가하여 저장하면 좋지만 일단 패쓰.

		// 플래그를 좀 더 다듬어야 할 듯?
		UINT flags = 0;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_PREFER_FLOW_CONTROL;// | D3DCOMPILE_ENABLE_STRICTNESS;
#else
		flags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		// defines를 ' '로 구분해서 vector에 넣고
		// 다시 이 vector의 값과 '1'을 macro로 추가한다.
		// urho의 경우 d3d11은 추가 macro를 설정하기도 했다.
		// 일단은 sparatan 구현으로 유지...
		std::vector<D3D_SHADER_MACRO> macros =
		{
			D3D_SHADER_MACRO{"COMPILEVS", m_Type == eShaderType::Vertex ? "1" : "0"},
			D3D_SHADER_MACRO{"COMPILEPS", m_Type == eShaderType::Pixel ? "1" : "0"}
		};
		// 매크로를 미리 설정한 후 컴파일한다면
		// 결국 하나의 파일로 여러 종류의 셰이더가 만들어 질 수 밖에 없다.
		// 그래서 이름에 매크로까지 추가한 것인가?
		for (const auto& macro : m_Defines)
		{
			macros.emplace_back(D3D_SHADER_MACRO{ macro.first.c_str(), macro.second.c_str() });
		}
		// 마지막은 무조건 nullptr, nullptr이어야 하나 보다.
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
				DV_LOG_ENGINE_ERROR("Shader::Compile - Shader Compile 실패: {:s}", (char*)pErrorMsg->GetBufferPointer());
				DV_RELEASE(pErrorMsg);
			}
			return false;
		}
		DV_RELEASE(pErrorMsg);
			
		// 전체 이름도 붙여서 출력하기.
		if (m_Type == eShaderType::Vertex)
			DV_LOG_ENGINE_DEBUG("Shader::Compile - 정점 셰이더를 컴파일 하였습니다.");
		else
			DV_LOG_ENGINE_DEBUG("Shader::Compile - 픽셀 셰이더를 컴파일 하였습니다.");

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
				DV_LOG_ENGINE_ERROR("Shader::Compile - 정점 셰이더({:s}) 생성에 실패하였습니다.", m_Name);
				return false;
			}

			DV_LOG_ENGINE_DEBUG("Shader::Compile - 정점 셰이더를 생성하였습니다.");
		}
		else
		{
			if (FAILED(m_pGraphics->GetDevice()->CreatePixelShader(
				m_pShaderBuffer->GetBufferPointer(),
				m_pShaderBuffer->GetBufferSize(),
				NULL,
				m_pPixelShader.ReleaseAndGetAddressOf())))
			{
				DV_LOG_ENGINE_ERROR("Shader::Compile - 픽셀 셰이더({:s}) 생성에 실패하였습니다.", m_Name);
				return false;
			}

			DV_LOG_ENGINE_DEBUG("Shader::Compile - 픽셀 셰이더를 생성하였습니다.");
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

	// 코드 통일성을 위해 컴파일, 셰이더 부분도 별도의 함수로 나누는 것이 나을 것 같다.
	// 그리고 이 함수도 public으로 바꾸자.
	// 그러면 결국 이들을 전부 호출하는 함수가 또 필요해진다.
	void Shader::parseParameters()
	{
		ID3D11ShaderReflection* pReflection = nullptr;
		if (FAILED(D3DReflect(m_pShaderBuffer->GetBufferPointer(), m_pShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection)))
		{
			return;
		}

		D3D11_SHADER_DESC shaderDesc;
		pReflection->GetDesc(&shaderDesc);

		// vs의 입력 파라미터?로 해시를 만드는 듯

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
				// 이건 urho가 가지는 네이밍 정책인 듯 하다.
				//if (varName[0] == 'c')
				{
				//	varName = varName.substr(1);
					m_Parameters[varName] = ShaderParameter(m_Type, varName, varDesc.StartOffset, varDesc.Size, cbRegister);
				}
			}
		}

		DV_RELEASE(pReflection);
	}

	// constant buffer의 개별 크기를 계산한다.
	// 버퍼 자체는 총 7개까지 만들 수 있다.
	void Shader::calculateConstantBufferSizes()
	{
		for (unsigned int i = 0; i < 7; ++i)
			m_ConstantBufferSizes[i] = 0;

		// parameter가 순서대로 저장된 것이 아니기에
		// 이렇게 계산해야 최종 크기가 나온다.
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