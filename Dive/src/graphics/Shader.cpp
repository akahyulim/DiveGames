#include "stdafx.h"
#include "Shader.h"
#include "Graphics.h"
#include "core/CoreDefs.h"
#include "core/FileUtils.h"
#include "core/StringUtils.h"

using Microsoft::WRL::ComPtr;

namespace Dive
{
	namespace ShaderUtils 
	{
		Microsoft::WRL::ComPtr<ID3DBlob> CompileShaderFile(const std::filesystem::path& filepath, eShaderType type)
		{
			ComPtr<ID3DBlob> compiledShader;
			ComPtr<ID3DBlob> errorMessage;

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
				DV_LOG(Shader, err, "잘못된 셰이더 타입을 전달받아 셰이더 파일 컴파일에 실패하였습니다.");
				return nullptr;
			}

			if (FAILED(D3DCompileFromFile(
				filepath.c_str(),
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				entryPoint.c_str(),
				profile.c_str(),
				D3D10_SHADER_ENABLE_STRICTNESS,
				0,
				compiledShader.GetAddressOf(),
				errorMessage.GetAddressOf())))
			{
				DV_LOG(Shader, err, "셰이더 컴파일 실패: {:s}", 
					errorMessage ? static_cast<char*>(errorMessage->GetBufferPointer()) : filepath.string().c_str());
				return nullptr;
			}

			return compiledShader;
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
				elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
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
				elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
				elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
				break;
			default:
				DV_LOG(Shader, err, "잘못된 입력 레이아웃 타입을 전달받아 입력 레이아웃 구성에 실패하였습니다.");
				elements.clear();
			}

			return elements;
		}
	}

	Shader::~Shader()
	{
		DV_RELEASE(m_PixelShader);
		DV_RELEASE(m_ComputeShader);
		DV_RELEASE(m_GeometryShader);
		DV_RELEASE(m_DomainShader);
		DV_RELEASE(m_HullShader);
		DV_RELEASE(m_VertexShader);
		DV_RELEASE(m_InputLayout);
	}

	bool Shader::Create(Microsoft::WRL::ComPtr<ID3DBlob> compiledShader, eShaderType type)
	{
		auto bufferPointer = compiledShader->GetBufferPointer();
		auto bufferSize = compiledShader->GetBufferSize();

		switch (type)
		{
		case eShaderType::Vertex:
			if (FAILED(Graphics::GetDevice()->CreateVertexShader(bufferPointer, bufferSize, NULL, &m_VertexShader)))
			{
				DV_LOG(Shader, err, "VertexShader 생성 실패");
				return false;
			}
			break;
		case eShaderType::Hull:
			if (FAILED(Graphics::GetDevice()->CreateHullShader(bufferPointer, bufferSize, NULL, &m_HullShader)))
			{
				DV_LOG(Shader, err, "HullShader 생성 실패");
				return false;
			}
			break;
		case eShaderType::Domain:
			if (FAILED(Graphics::GetDevice()->CreateDomainShader(bufferPointer, bufferSize, NULL, &m_DomainShader)))
			{
				DV_LOG(Shader, err, "DomainShader 생성 실패");
				return false;
			}
			break;
		case eShaderType::Geometry:
			if (FAILED(Graphics::GetDevice()->CreateGeometryShader(bufferPointer, bufferSize, NULL, &m_GeometryShader)))
			{
				DV_LOG(Shader, err, "GeometryShader 생성 실패");
				return false;
			}
			break;
		case eShaderType::Pixel:
			if (FAILED(Graphics::GetDevice()->CreatePixelShader(bufferPointer, bufferSize, NULL, &m_PixelShader)))
			{
				DV_LOG(Shader, err, "Pixel Shader 생성 실패");
				return false;
			}
			break;
		case eShaderType::Compute:
			if (FAILED(Graphics::GetDevice()->CreateComputeShader(bufferPointer, bufferSize, NULL, &m_ComputeShader)))
			{
				DV_LOG(Shader, err, "ComputeShader 생성 실패");
				return false;
			}
			break;
		default:
			DV_LOG(Shader, err, "셰이더 생성 실패: 잘못된 셰이더 타입 전달");
			return false;
		}

		m_Type = type;
		return true;
	}

	bool Shader::CreateInputLayout(Microsoft::WRL::ComPtr<ID3DBlob> blob, eInputLayout layout)
	{
		if (!blob || layout == eInputLayout::None)
		{
			DV_LOG(Shader, err, "InputLayout 생성 실패: 유효하지 않은 매개변수 전달");
			return false;
		}
	
		auto inputElements = ShaderUtils::GetInputElements(layout);

		if (FAILED(Graphics::GetDevice()->CreateInputLayout(
			inputElements.data(),
			static_cast<UINT32>(inputElements.size()),
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			&m_InputLayout)))
		{
			DV_LOG(Shader, err, "InputLayout 생성 실패");
			return false;
		}

		return true;
	}

	std::shared_ptr<Shader> Shader::Generate(const std::filesystem::path& filepath, eShaderType type, eInputLayout layout)
	{
		auto compiledShader = ShaderUtils::CompileShaderFile(filepath, type);
		if (!compiledShader)
			return nullptr;

		auto shader = std::make_shared<Shader>();
		if (!shader->Create(compiledShader, type))
			return nullptr;

		if (type == eShaderType::Vertex)
		{
			if (!shader->CreateInputLayout(compiledShader, layout))
				return nullptr;
		}

		return shader;
	}
}