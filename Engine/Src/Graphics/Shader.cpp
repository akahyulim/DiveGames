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
		UINT flags = 0;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_PREFER_FLOW_CONTROL;
#else
		flags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		// defines를 ' '로 구분해서 vector에 넣고
		// 다시 이 vector의 값과 '1'을 macro로 추가한다.
		// urho의 경우 d3d11은 추가 macro를 설정하기도 했다.
		// 일단은 sparatan 구현으로 유지...
		std::vector<D3D_SHADER_MACRO> macros =
		{
			D3D_SHADER_MACRO{"VS", m_Type == eShaderType::Vertex ? "1" : "0"},
			D3D_SHADER_MACRO{"PS", m_Type == eShaderType::Pixel ? "1" : "0"}
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
			nullptr,
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
}