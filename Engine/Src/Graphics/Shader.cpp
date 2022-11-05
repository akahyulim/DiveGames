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
		DV_ASSERT(m_pGraphics);
	}

	Shader::~Shader()
	{
		DV_LOG_ENGINE_TRACE("Shader 소멸자 호출");

		Release();
	}

	bool Shader::Compile(const std::string& filepath)
	{
		Release();

		if (!m_pGraphics || !m_pGraphics->IsInitialized())
			return false;

		if (!std::filesystem::exists(filepath))
		{
			DV_LOG_ENGINE_ERROR("Shader::Compile - 전달받은 파일({:s})이 존재하지 않습니다.", filepath);
			return false;
		}

		// 경로, 이름 등으로 나누어 저장. => 리소스 쪽에 구현?
		// 이름의 경우 매크로까지 추가하여 저장하면 좋지만 일단 패쓰.

		ID3DBlob* pShaderBlob = nullptr;
		ID3DBlob* pErrorMsg = nullptr;

		UINT flags = 0;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_PREFER_FLOW_CONTROL;
#else
		flags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		std::vector<D3D_SHADER_MACRO> macros =
		{
			D3D_SHADER_MACRO{"VS", m_Type == eShaderType::Vertex ? "1" : "0"},
			D3D_SHADER_MACRO{"PS", m_Type == eShaderType::Pixel ? "1" : "0"}
		};
		for (const auto& macro : m_Defines)
		{
			macros.emplace_back(D3D_SHADER_MACRO{ macro.first.c_str(), macro.second.c_str() });
		}
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

		if (FAILED(D3DCompileFromFile(
			fileName.c_str(),
			macros.data(),
			nullptr,
			pEntryPoint,
			pTarget,
			flags,
			0,
			&pShaderBlob,
			&pErrorMsg
		)))
		{
			if (pErrorMsg)
			{
				DV_LOG_ENGINE_ERROR("Shader::Compile - Shader Compile 실패: {:s}", (char*)pErrorMsg->GetBufferPointer());
				DV_RELEASE(pErrorMsg);
			}
			DV_RELEASE(pShaderBlob);
			return false;
		}
		else
		{
			// 전체 이름도 붙여서 출력하기.
			if (m_Type == eShaderType::Vertex)
				DV_LOG_ENGINE_DEBUG("Shader::Compile - 정점 셰이더를 컴파일 하였습니다.");
			else
				DV_LOG_ENGINE_DEBUG("Shader::Compile - 픽셀 셰이더를 컴파일 하였습니다.");

			ID3DBlob* pStrippedCode = nullptr;
			D3DStripShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(),
				D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_TEST_BLOBS, &pStrippedCode);

			m_ByteCode.resize(pStrippedCode->GetBufferSize());
			memcpy(m_ByteCode.data(), pStrippedCode->GetBufferPointer(), m_ByteCode.size());

			DV_RELEASE(pStrippedCode);
			DV_RELEASE(pShaderBlob);
			DV_RELEASE(pErrorMsg);
		}

		auto* pDevice = m_pGraphics->GetDevice();
		if (m_Type == eShaderType::Vertex)
		{
			if (FAILED(pDevice->CreateVertexShader(
				m_ByteCode.data(),
				m_ByteCode.size(),
				NULL,
				m_pVertexShader.GetAddressOf())))
			{
				DV_LOG_ENGINE_ERROR("Shader::Compile - 정점 셰이더({:s}) 생성에 실패하였습니다.", m_Name);
				return false;
			}
		}
		else
		{
			if (FAILED(pDevice->CreatePixelShader(
				m_ByteCode.data(),
				m_ByteCode.size(),
				NULL,
				m_pPixelShader.GetAddressOf())))
			{
				DV_LOG_ENGINE_ERROR("Shader::Compile - 픽셀 셰이더({:s}) 생성에 실패하였습니다.", m_Name);
				return false;
			}
		}

		return true;
	}

	void Shader::Release()
	{
		m_pPixelShader.Reset();
		m_pVertexShader.Reset();

		m_ByteCode.clear();
		m_ByteCode.shrink_to_fit();
	}

	void* Shader::GetShader()
	{
		return m_Type == eShaderType::Vertex ?
			static_cast<void*>(m_pVertexShader.Get()) : static_cast<void*>(m_pPixelShader.Get());
	}
}