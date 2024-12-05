#include "stdafx.h"
#include "Shader.h"
#include "Graphics.h"
#include "core/CoreDefs.h"
#include "core/FileUtils.h"
#include "core/StringUtils.h"
#include "core/Engine.h"

namespace Dive
{
	ID3DBlob* CompileFromFile(const std::string& fileName, eShaderType type)
	{
		ID3DBlob* pShaderBlob = nullptr;
		ID3DBlob* pErrorMsgs = nullptr;

		std::string entryPoint;
		std::string profile;
		switch (type)
		{
		case eShaderType::Vertex:
			entryPoint = "MainVS";
			profile = "vs_5_0";
			break;
		case eShaderType::Hull:
			entryPoint = "MainHS";
			profile = "hs_5_0";
			break;
		case eShaderType::Domain:
			entryPoint = "MainDS";
			profile = "ds_5_0";
			break;
		case eShaderType::Geometry:
			entryPoint = "MainGS";
			profile = "gs_5_0";
			break;
		case eShaderType::Pixel:
			entryPoint = "MainPS";
			profile = "ps_5_0";
			break;
		case eShaderType::Compute:
			entryPoint = "MainCS";
			profile = "cs_5_0";
			break;
		default:
			DV_LOG(Shader, err, "�߸��� ���̴� Ÿ���� ���޹޾� ���̴� ���� �����Ͽ� �����Ͽ����ϴ�.");
			return nullptr;
		}

		std::wstring filePath = StringUtils::StringToWString(fileName);

		if (FAILED(D3DCompileFromFile(
			filePath.c_str(),
			nullptr, //macros.data(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint.c_str(),
			profile.c_str(),
			D3D10_SHADER_ENABLE_STRICTNESS,
			0,
			&pShaderBlob,
			&pErrorMsgs)))
		{
			if (pErrorMsgs)
				DV_LOG(Shader, err, "{:s}", (char*)pErrorMsgs->GetBufferPointer());
			else
				DV_LOG(Shader, err, "���̴� ����({:s}) �����Ͽ� �����Ͽ����ϴ�.", fileName);

			DV_RELEASE(pErrorMsgs);
			DV_RELEASE(pShaderBlob);

			return nullptr;
		}

		return pShaderBlob;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements(eInputLayout layout)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> elements;

		switch (layout)
		{
		case eInputLayout::Pos:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		case eInputLayout::Pos_Tex:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		case eInputLayout::Pos_Tex_Nor:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0});
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		case eInputLayout::Static_Model:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		case eInputLayout::Skinned_Model:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			//elements.emplace_back("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		default:
			DV_LOG(Shader, err, "�߸��� �Է� ���̾ƿ� Ÿ���� ���޹޾� �Է� ���̾ƿ� ������ �����Ͽ����ϴ�.");
			elements.clear();
		}

		return elements;
	}

	Shader::Shader()
		: m_Type(eShaderType::Undefined)
		, m_pVertexShader(nullptr)
		, m_pHullShader(nullptr)
		, m_pDomainShader(nullptr)
		, m_pComputeShader(nullptr)
		, m_pGeometryShader(nullptr)
		, m_pPixelShader(nullptr)
		, m_pInputLayout(nullptr)
	{
	}
	
	Shader::~Shader()
	{
		DV_RELEASE(m_pPixelShader);
		DV_RELEASE(m_pComputeShader);
		DV_RELEASE(m_pGeometryShader);
		DV_RELEASE(m_pDomainShader);
		DV_RELEASE(m_pHullShader);
		DV_RELEASE(m_pVertexShader);
		DV_RELEASE(m_pInputLayout);
	}

	// pShaderBlob�� ������ release ����� �Ѵ�.
	bool Shader::CompileAndCreateShader(const std::string& filename, eShaderType type, eInputLayout layout)
	{
		ID3D10Blob* pShaderBlob = CompileFromFile(filename, type);
	
		switch (type)
		{
		case eShaderType::Vertex:
			if (FAILED(GEngine->GetDevice()->CreateVertexShader(
				pShaderBlob->GetBufferPointer(), 
				pShaderBlob->GetBufferSize(), 
				NULL, 
				&m_pVertexShader)))
			{
				DV_LOG(Shader, err, "VertexShader ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (layout != eInputLayout::None)
			{
				if (!createInputLayout(pShaderBlob, layout))
					return false;
			}
			break;
		case eShaderType::Hull:
			if (FAILED(GEngine->GetDevice()->CreateHullShader(
				pShaderBlob->GetBufferPointer(), 
				pShaderBlob->GetBufferSize(), 
				NULL, 
				&m_pHullShader)))
			{
				DV_LOG(Shader, err, "HullShader ������ �����Ͽ����ϴ�.");
				return false;
			}
			break;
		case eShaderType::Domain:
			if (FAILED(GEngine->GetDevice()->CreateDomainShader(
				pShaderBlob->GetBufferPointer(), 
				pShaderBlob->GetBufferSize(), 
				NULL, 
				&m_pDomainShader)))
			{
				DV_LOG(Shader, err, "DomainShader ������ �����Ͽ����ϴ�.");
				return false;
			}
			break; 
		case eShaderType::Geometry:
				if (FAILED(GEngine->GetDevice()->CreateGeometryShader(
					pShaderBlob->GetBufferPointer(),
					pShaderBlob->GetBufferSize(),
					NULL,
					&m_pGeometryShader)))
				{
					DV_LOG(Shader, err, "GeometryShader ������ �����Ͽ����ϴ�.");
					return false;
				}
				break;
		case eShaderType::Pixel:
			if (FAILED(GEngine->GetDevice()->CreatePixelShader(
				pShaderBlob->GetBufferPointer(), 
				pShaderBlob->GetBufferSize(), 
				NULL, 
				&m_pPixelShader)))
			{
				DV_LOG(Shader, err, "Pixel Shader ������ �����Ͽ����ϴ�.");
				return false;
			}
			break;
		case eShaderType::Compute:
			if (FAILED(GEngine->GetDevice()->CreateComputeShader(
				pShaderBlob->GetBufferPointer(),
				pShaderBlob->GetBufferSize(),
				NULL,
				&m_pComputeShader)))
			{
				DV_LOG(Shader, err, "ComputeShader ������ �����Ͽ����ϴ�.");
				return false;
			}
			break;
		default:
			DV_LOG(Shader, err, "�߸��� ���̴� Ÿ���� ���޹޾� ���̴� ������ �����Ͽ����ϴ�.");
			return false;
		}

		pShaderBlob->Release();

		m_Type = type;

		return true;
	}
	
	bool Shader::createInputLayout(ID3D10Blob* pShaderBlob, eInputLayout layout)
	{
		DV_ASSERT(Shader, pShaderBlob);
		//DV_ASSERT(Shader, layout != eInputLayout::None);

		auto inputElements = GetInputElements(layout);

		if (FAILED(GEngine->GetDevice()->CreateInputLayout(
			inputElements.data(),
			static_cast<UINT>(inputElements.size()),
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			&m_pInputLayout)))
		{
			DV_LOG(Shader, err, "InputLayout ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}
}