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
		DV_LOG_ENGINE_TRACE("Shader �Ҹ��� ȣ��");

		Release();
	}

	bool Shader::Compile(const std::string& filepath)
	{
		Release();

		if (!m_pGraphics || !m_pGraphics->IsInitialized())
			return false;

		if (!std::filesystem::exists(filepath))
		{
			DV_LOG_ENGINE_ERROR("Shader::Compile - ���޹��� ����({:s})�� �������� �ʽ��ϴ�.", filepath);
			return false;
		}

		// ���, �̸� ������ ������ ����. => ���ҽ� �ʿ� ����?
		// �̸��� ��� ��ũ�α��� �߰��Ͽ� �����ϸ� ������ �ϴ� �о�.

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
				DV_LOG_ENGINE_ERROR("Shader::Compile - Shader Compile ����: {:s}", (char*)pErrorMsg->GetBufferPointer());
				DV_RELEASE(pErrorMsg);
			}
			DV_RELEASE(pShaderBlob);
			return false;
		}
		else
		{
			// ��ü �̸��� �ٿ��� ����ϱ�.
			if (m_Type == eShaderType::Vertex)
				DV_LOG_ENGINE_DEBUG("Shader::Compile - ���� ���̴��� ������ �Ͽ����ϴ�.");
			else
				DV_LOG_ENGINE_DEBUG("Shader::Compile - �ȼ� ���̴��� ������ �Ͽ����ϴ�.");

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
				DV_LOG_ENGINE_ERROR("Shader::Compile - ���� ���̴�({:s}) ������ �����Ͽ����ϴ�.", m_Name);
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
				DV_LOG_ENGINE_ERROR("Shader::Compile - �ȼ� ���̴�({:s}) ������ �����Ͽ����ϴ�.", m_Name);
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