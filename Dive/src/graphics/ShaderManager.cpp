#include "stdafx.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "Graphics.h"

namespace Dive
{
	std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> ShaderManager::s_programCache;

	static std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements(eInputLayout layout)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> elements;

		switch (layout)
		{
		
		case eInputLayout::Unlit:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		case eInputLayout::Lit:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		case eInputLayout::Skinned:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		default:
			DV_LOG(Shader, err, "잘못된 입력 레이아웃 타입 전달");
			break;
		}

		return elements;
	}

	bool ShaderManager::Initialize()
	{
		s_programCache["ForwardLight"] = CreateShaderProgram("ForwardLight", eInputLayout::Lit);
		if (!s_programCache["ForwardLight"])
		{
			DV_LOG(ShaderManager, err, "ForwardLight ShaderProgram 생성 실패");
			return false;
		}

		s_programCache["Unlit"] = CreateShaderProgram("Unlit", eInputLayout::Unlit);
		if (!s_programCache["Unlit"])
		{
			DV_LOG(ShaderManager, err, "Unlit ShaderProgram 생성 실패");
			return false;
		}

		return true;
	}

	std::shared_ptr<ShaderProgram> ShaderManager::GetProgram(const std::string& name)
	{
		auto it = s_programCache.find(name);
		if (it == s_programCache.end())
			return nullptr;

		return it->second;
	}

	std::shared_ptr<ShaderProgram> ShaderManager::CreateShaderProgram(const std::string& name, eInputLayout type)
	{
		auto vs = std::make_shared<VertexShader>();
		if (!vs->LoadFromFile("Assets/Shaders/" + name + "VS.cso"))
			return nullptr;

		auto ps = std::make_shared<PixelShader>();
		if (!ps->LoadFromFile("Assets/Shaders/" + name + "PS.cso"))
			return nullptr;

		auto il = CreateInputLayout(vs, type);

		return std::make_shared<ShaderProgram>(vs, ps, il, name);
	}

	Microsoft::WRL::ComPtr<ID3D11InputLayout> ShaderManager::CreateInputLayout(std::shared_ptr<VertexShader> vs, eInputLayout type)
	{
		assert(vs && vs->GetBytecode());

		Microsoft::WRL::ComPtr<ID3D11InputLayout> il;
		
		auto inputElements = GetInputElements(type);
		auto byteCode = vs->GetBytecode();
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

		auto hr = Graphics::GetDevice()->CreateInputLayout(
			inputElements.data(),
			static_cast<UINT>(inputElements.size()),
			byteCode->GetBufferPointer(),
			byteCode->GetBufferSize(),
			il.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(ShaderManager, err, "[::InputLayout] 생성 실패: {}", ErrorUtils::ToVerbose(hr));
			return nullptr;
		}

		return il;
	}
}