#include "DivePch.h"
#include "ShaderVariation.h"
#include "InputLayout.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

namespace Dive
{
	ShaderVariation::ShaderVariation()
		: m_ShaderType(eShaderType::UnknownShader),
		m_pShaderBuffer(nullptr),
		m_pShaderResource(nullptr),
		m_pInputLayout(nullptr)
	{
	}


	ShaderVariation::~ShaderVariation()
	{
		DV_RELEASE(m_pShaderBuffer);

		DV_DELETE(m_pInputLayout);

		if (m_ShaderType == eShaderType::VertexShader)
			((ID3D11VertexShader*)m_pShaderResource)->Release();
		else if (m_ShaderType == eShaderType::VertexShader)
			((ID3D11PixelShader*)m_pShaderResource)->Release();
		else if (m_ShaderType == eShaderType::VertexShader)
			((ID3D11ComputeShader*)m_pShaderResource)->Release();
		m_pShaderResource = nullptr;

		DV_CORE_TRACE("ShaderVariation �Ҹ��� ȣ�� - {0:s} - {1:d}", m_Name, m_ID);
	}

	bool ShaderVariation::CompileAndCreate(eShaderType type, const std::string& filePath, eVertexType vertexType)
	{
		if (!Compile(type, filePath))
			return false;

		if (!CreateShader(type, vertexType))
			return false;

		return true;
	}

	// �߰��� ��ũ�θ� ������ �� �־�� �Ѵ�.
	bool ShaderVariation::Compile(eShaderType type, const std::string& filePath)
	{
		DV_ASSERT(type != eShaderType::UnknownShader);

		if (!FileSystem::FileExists(filePath))
		{
			DV_CORE_ERROR("���̴� ����({:s})�� �������� �ʾ� �����Ͽ� �����Ͽ����ϴ�.", filePath);
			return false;
		}

		std::wstring fileName = FileSystem::StringToWstring(filePath);

		std::vector<D3D_SHADER_MACRO> macros =
		{
			D3D_SHADER_MACRO{"COMPILEVS", type == eShaderType::VertexShader ? "1" : "0"},
			D3D_SHADER_MACRO{"COMPILEPS", type == eShaderType::PixelShader ? "1" : "0"},
			D3D_SHADER_MACRO{"COMPILECS", type == eShaderType::ComputeShader ? "1" : "0"}
		};

		macros.emplace_back(D3D_SHADER_MACRO{ nullptr, nullptr });

		const char* pEntrypoint = nullptr;
		const char* pTarget = nullptr;

		if (type == eShaderType::VertexShader)
		{
			pEntrypoint = "MainVS";
			pTarget = "vs_5_0";
		}
		else if (type == eShaderType::PixelShader)
		{
			pEntrypoint = "MainPS";
			pTarget = "ps_5_0";
		}
		else
		{
			pEntrypoint = "MainCS";
			pTarget = "cs_5_0";
		}

		ID3D10Blob* pErrorMsgs = nullptr;

		if (FAILED(D3DCompileFromFile(
			fileName.c_str(),
			macros.data(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			pEntrypoint,
			pTarget,
			D3D10_SHADER_ENABLE_STRICTNESS,
			0,
			&m_pShaderBuffer,
			&pErrorMsgs)))
		{
			DV_CORE_ERROR("���̴� ����({:s}) �����Ͽ� �����Ͽ����ϴ�.", filePath);
			if (pErrorMsgs)
				DV_CORE_ERROR("{:s}", (char*)pErrorMsgs->GetBufferPointer());
			pErrorMsgs->Release();

			DV_RELEASE(m_pShaderBuffer);

			return false;
		}
	
		m_ShaderType = type;
		m_FilePath = filePath;
		SetName(FileSystem::GetFileName(filePath));

		return true;
	}

	bool ShaderVariation::CreateShader(eShaderType type, eVertexType vertexType)
	{
		DV_ASSERT(type != eShaderType::UnknownShader);

		if (!m_pShaderBuffer)
		{
			DV_CORE_ERROR("���̴� ���۰� ����־� ���̴� ��ü ������ �����Ͽ����ϴ�.");
			return false;
		}

		auto pDevice = Graphics::GetDevice();
		DV_ASSERT(pDevice);

		if (m_ShaderType != type)
			m_ShaderType = type;

		if (m_ShaderType == eShaderType::VertexShader)
		{
			if (FAILED(pDevice->CreateVertexShader(
				m_pShaderBuffer->GetBufferPointer(),
				m_pShaderBuffer->GetBufferSize(),
				nullptr,
				reinterpret_cast<ID3D11VertexShader**>(&m_pShaderResource))))
			{
				DV_CORE_ERROR("VertexShader ��ü ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (!CreateInputLayout(vertexType))
				return false;
		}
		else if (m_ShaderType == eShaderType::PixelShader)
		{
			if (FAILED(pDevice->CreatePixelShader(
				m_pShaderBuffer->GetBufferPointer(),
				m_pShaderBuffer->GetBufferSize(),
				nullptr,
				reinterpret_cast<ID3D11PixelShader**>(&m_pShaderResource))))
			{
				DV_CORE_ERROR("PixelShader ��ü ������ �����Ͽ����ϴ�.");
				return false;
			}
		}
		else if (m_ShaderType == eShaderType::ComputeShader)
		{
			if (FAILED(pDevice->CreateComputeShader(
				m_pShaderBuffer->GetBufferPointer(),
				m_pShaderBuffer->GetBufferSize(),
				nullptr,
				reinterpret_cast<ID3D11ComputeShader**>(&m_pShaderResource))))
			{
				DV_CORE_ERROR("ComputeShader ��ü ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		return true;
	}

	bool ShaderVariation::CreateInputLayout(eVertexType vertexType)
	{
		if (!m_pShaderBuffer)
		{
			DV_CORE_ERROR("���̴� ���۰� ����־� ID3D11InputLayout ������ �����Ͽ����ϴ�.");
			return false;
		}

		if (vertexType == eVertexType::Undefined)
		{
			DV_CORE_INFO("{:s} - VertexType ������ ���ǵ��� �ʾ� InputLayout�� �������� �ʾҽ��ϴ�.", GetName());
			return true;
		}

		m_pInputLayout = new InputLayout;
		if (!m_pInputLayout->Create(m_pShaderBuffer, vertexType))
			return false;

		return true;
	}

	InputLayout* ShaderVariation::GetInputLayout() const
	{
		return m_ShaderType == eShaderType::VertexShader ? m_pInputLayout : nullptr;
	}
}