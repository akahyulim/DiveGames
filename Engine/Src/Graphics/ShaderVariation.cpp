#include "divepch.h"
#include "ShaderVariation.h"
#include "Shader.h"
#include "Graphics.h"
#include "GraphicsDefs.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	uint32_t SemanticToVertexElement(const char* pSemantic)
	{
		uint32_t i = 0;
		while (ELEMENT_SEMANTICNAMES[i])
		{
			if (0 == strcmp(pSemantic, ELEMENT_SEMANTICNAMES[i]))
				return i;
			++i;
		}

		return eVertexElementSemantic::MAX_VERTEX_ELEMENT_SEMANTICS;
	}

	ShaderVariation::ShaderVariation(Shader* pOwner, eShaderType type)
		: m_pGraphics(pOwner->GetSubsystem<Graphics>()),
		m_pOwner(pOwner),
		m_Type(type),
		m_pCompileBuffer(nullptr),
		m_pVertexShader(nullptr),
		m_pPixelShader(nullptr),
		m_SemanticsHash(0)
	{
		for (uint32_t i = 0; i < 16; ++i)
			m_bUseTextureUnits[i] = false;

		for (uint32_t i = 0; i < 7; ++i)
			m_ConstantBufferSizes[i] = 0;

		DV_ASSERT(m_pGraphics->IsInitialized());
	}

	ShaderVariation::~ShaderVariation()
	{
		DV_RELEASE(m_pCompileBuffer);
		DV_RELEASE(m_pPixelShader);
		DV_RELEASE(m_pVertexShader);

		// define도 붙이기
		DV_LOG_ENGINE_TRACE("ShaderVariation 소멸 완료({:s})", m_Type == eShaderType::Vertex ? "VertexShader" : "PixelShader");
	}

	bool ShaderVariation::CompileAndCreate()
	{
		if (!Compile())
			return false;
		
		parseParameters();
		calculateConstantBufferSizes();

		if (m_Type == eShaderType::Vertex)
		{
			if (!CreateVertexShader())
				return false;
		}
		else if (m_Type == eShaderType::Pixel)
		{
			if (!CreatePixelShader())
				return false;
		}

		return true;
	}

	bool ShaderVariation::Compile()
	{
		const std::string& sourceCode = m_pOwner->GetSourceCode(m_Type);
		if (sourceCode.empty())
		{
			DV_LOG_ENGINE_ERROR("ShaderVariation::Compile - 셰이더 컴파일 코드가 존재하지 않습니다.");
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

		DV_RELEASE(m_pCompileBuffer);

		ID3D10Blob* pErrorMsg = nullptr;
		if (FAILED(D3DCompile(
			sourceCode.c_str(),
			sourceCode.size(),
			m_pOwner->GetFilePath().c_str(),
			macros.data(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			pEntryPoint,
			pTarget,
			flags,
			0,
			&m_pCompileBuffer,
			&pErrorMsg)))
		{
			if (pErrorMsg)
			{
				DV_LOG_ENGINE_ERROR("ShaderVariation::Compile - ShaderVariation Compile 실패: {:s}", (char*)pErrorMsg->GetBufferPointer());
				DV_RELEASE(pErrorMsg);
			}

			DV_RELEASE(m_pCompileBuffer);

			return false;
		}
		DV_RELEASE(pErrorMsg);

		// 전체 이름도 붙여서 출력하기.
		if (m_Type == eShaderType::Vertex)
			DV_LOG_ENGINE_TRACE("ShaderVariation::Compile - 정점 셰이더를 컴파일 하였습니다.");
		else
			DV_LOG_ENGINE_TRACE("ShaderVariation::Compile - 픽셀 셰이더를 컴파일 하였습니다.");

		return true;
	}

	bool ShaderVariation::CreateVertexShader()
	{
		if (!m_pCompileBuffer)
		{
			DV_LOG_ENGINE_ERROR("ShaderVariation::CreateVertexShader - 셰이더 컴파일 버퍼가 존재하지 않습니다.");
			return false;
		}

		DV_RELEASE(m_pVertexShader);

		if (FAILED(m_pGraphics->GetDevice()->CreateVertexShader(
			m_pCompileBuffer->GetBufferPointer(),
			m_pCompileBuffer->GetBufferSize(),
			nullptr,
			&m_pVertexShader)))
		{
			DV_RELEASE(m_pVertexShader);
			DV_LOG_ENGINE_ERROR("ShaderVariation::Compile - 정점 셰이더({:s}) 생성에 실패하였습니다.", m_Name);
			return false;
		}

		DV_LOG_ENGINE_TRACE("ShaderVariation::Compile - 정점 셰이더를 생성하였습니다.");
		
		return true;
	}

	bool ShaderVariation::CreatePixelShader()
	{
		if (!m_pCompileBuffer)
		{
			DV_LOG_ENGINE_ERROR("ShaderVariation::CreatePixelShader - 셰이더 컴파일 버퍼가 존재하지 않습니다.");
			return false;
		}
		
		DV_RELEASE(m_pPixelShader);

		if (FAILED(m_pGraphics->GetDevice()->CreatePixelShader(
			m_pCompileBuffer->GetBufferPointer(),
			m_pCompileBuffer->GetBufferSize(),
			NULL,
			&m_pPixelShader)))
		{
			DV_RELEASE(m_pPixelShader);
			DV_LOG_ENGINE_ERROR("ShaderVariation::Compile - 픽셀 셰이더({:s}) 생성에 실패하였습니다.", m_Name);
			return false;
		}

		DV_LOG_ENGINE_TRACE("ShaderVariation::Compile - 픽셀 셰이더를 생성하였습니다.");

		return true;
	}

	void* ShaderVariation::GetShader()
	{
		return m_Type == eShaderType::Vertex ?
			static_cast<void*>(m_pVertexShader) : static_cast<void*>(m_pPixelShader);
	}

	bool ShaderVariation::HasParameter(const std::string& name) const
	{
		if (!m_Parameters.empty())
		{
			auto it = m_Parameters.find(name);
			return it != m_Parameters.end();
		}

		return false;
	}

	void ShaderVariation::parseParameters()
	{
		ID3D11ShaderReflection* pReflection = nullptr;
		if (FAILED(D3DReflect(m_pCompileBuffer->GetBufferPointer(), m_pCompileBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection)))
		{
			DV_LOG_ENGINE_ERROR("ShaderVariation::parseParameters - 파라미터 파싱에 실패하였습니다.");
			return;
		}

		D3D11_SHADER_DESC shaderDesc;
		pReflection->GetDesc(&shaderDesc);

		if (m_Type == eShaderType::Vertex)
		{
			// 사실 이 해시를 생성하는 과정은 유의미하지 않다.
			// VertexBuffer의 해시에 이 해시를 더해 새로운 조합의 해시를 만드는데 사용될 뿐이다.
			m_SemanticsHash = 0;
			for (uint32_t i = 0; i < shaderDesc.InputParameters; ++i)
			{
				D3D11_SIGNATURE_PARAMETER_DESC desc;
				pReflection->GetInputParameterDesc(static_cast<UINT>(i), &desc);

				m_SemanticsHash <<= 3;
				m_SemanticsHash += (SemanticToVertexElement(desc.SemanticName) + 1) + desc.SemanticIndex;
			}
		}

		std::map<std::string, uint32_t> cbRegisterMap;

		for (uint32_t i = 0; i < shaderDesc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC desc;
			pReflection->GetResourceBindingDesc(i, &desc);
			std::string resourceName(desc.Name);
			
			if (desc.Type == D3D_SIT_CBUFFER)
				cbRegisterMap[resourceName] = desc.BindPoint;
			else if (desc.Type == D3D_SIT_SAMPLER && desc.BindPoint < 16)
				m_bUseTextureUnits[desc.BindPoint] = true;
		}

		for (uint32_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
		{
			auto pConstantBuffer = pReflection->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC desc;
			pConstantBuffer->GetDesc(&desc);

			uint32_t cbRegister = cbRegisterMap[std::string(desc.Name)];

			for (uint32_t j = 0; j < desc.Variables; ++j)
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
	void ShaderVariation::calculateConstantBufferSizes()
	{
		for (uint32_t i = 0; i < 7; ++i)
			m_ConstantBufferSizes[i] = 0;

		// parameter가 순서대로 저장된 것이 아니기에
		// 이렇게 계산해야 최종 크기가 나온다.
		auto it = m_Parameters.begin();
		for (it; it != m_Parameters.end(); ++it)
		{
			if (it->second.Index < 7)
			{
				uint32_t oldSize = m_ConstantBufferSizes[it->second.Index];
				uint32_t paramEnd = it->second.Offset + it->second.Size;
				if (paramEnd > oldSize)
					m_ConstantBufferSizes[it->second.Index] = paramEnd;
			}
		}
	}
}