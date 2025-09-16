#include "stdafx.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "Graphics.h"

namespace Dive
{
	std::unordered_map<std::string, std::pair<std::shared_ptr<VertexShader>, Microsoft::WRL::ComPtr<ID3D11InputLayout>>> ShaderManager::s_vss;
	std::unordered_map<std::string, std::shared_ptr<PixelShader>> ShaderManager::s_pss;
	std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> ShaderManager::s_spCache;

	static std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements(eInputLayout layout)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> elements;

		switch (layout)
		{
		
		case eInputLayout::Unlit:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
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
		// vertex shader and input layout
		if (!CreateVertexShaderAndInputLayout("Assets/Shaders/UnlitVS.cso", eInputLayout::Unlit))
		{
			DV_LOG(ShaderManager, err, "UnlitVS 생성 실패");
			return false;
		}
		if(!CreateVertexShaderAndInputLayout("Assets/Shaders/LitVS.cso", eInputLayout::Lit))
		{
			DV_LOG(ShaderManager, err, "LitVS 생성 실패");
			return false;
		}
		if (!CreateVertexShaderAndInputLayout("Assets/Shaders/ResolveSceneVS.cso", eInputLayout::None))
		{
			DV_LOG(ShaderManager, err, "ResolbeSceneVS 생성 실패");
			return false;
		}

		// pixel shader
		if(!CreatePixelShader("Assets/Shaders/UnlitPS.cso"))
		{
			DV_LOG(ShaderManager, err, "UnlitPS 생성 실패");
			return false;
		}
		if(!CreatePixelShader("Assets/Shaders/LegacyPS.cso"))
		{
			DV_LOG(ShaderManager, err, "LegacyPS 생성 실패");
			return false;
		}
		if (!CreatePixelShader("Assets/Shaders/ResolveScenePS.cso"))
		{
			DV_LOG(ShaderManager, err, "ResolveScenePS 생성 실패");
			return false;
		}

		// shader program
		if(!CreateShaderProgram("UnlitVS", "UnlitPS", "Unlit"))
		{
			DV_LOG(ShaderManager, err, "Unlit ShaderProgram 생성 실패");
			return false;
		}
		if(!CreateShaderProgram("LitVS", "LegacyPS", "Legacy"))
		{
			DV_LOG(ShaderManager, err, "Legacy ShaderProgram 생성 실패");
			return false;
		}
		if (!CreateShaderProgram("ResolveSceneVS", "ResolveScenePS", "ResolveScene"))
		{
			DV_LOG(ShaderManager, err, "ResolveScene ShaderProgram 생성 실패");
			return false;
		}


		return true;
	}

	std::shared_ptr<ShaderProgram> ShaderManager::GetProgram(const std::string& name)
	{
		auto it = s_spCache.find(name);
		if (it == s_spCache.end())
		{
			DV_LOG(ShaderProgram, warn, "존재하지 않는 셰이더 프로그램 요청: {}", name);
			return nullptr;
		}
		return it->second;
	}

	bool ShaderManager::CreateVertexShaderAndInputLayout(const std::filesystem::path& filepath, eInputLayout type)
	{
		auto vs = std::make_shared<VertexShader>();
		if (!vs->LoadFromFile(filepath.c_str()))
		{
			DV_LOG(ShaderManager, err, "[::CreateVertexShaderAndInputLayout] VS 생성 실패");
			return false;
		}

		if (type != eInputLayout::None)
		{
			Microsoft::WRL::ComPtr<ID3D11InputLayout> il;
			auto inputElements = GetInputElements(type);
			auto byteCode = vs->GetBytecode();
			auto hr = Graphics::GetDevice()->CreateInputLayout(
				inputElements.data(),
				static_cast<UINT>(inputElements.size()),
				byteCode->GetBufferPointer(),
				byteCode->GetBufferSize(),
				il.GetAddressOf());
			if (FAILED(hr))
			{
				DV_LOG(ShaderManager, err, "[::CreateVertexShaderAndInputLayout] InputLayout 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return false;
			}

			s_vss[vs->GetName()] = { vs, il };
		}
		else 
			s_vss[vs->GetName()] = { vs, nullptr };

		return true;
	}

	bool ShaderManager::CreatePixelShader(const std::filesystem::path& filepath)
	{
		auto ps = std::make_shared<PixelShader>();
		if (!ps->LoadFromFile(filepath))
		{
			DV_LOG(ShaderManager, err, "[::CreatePixelShader] PS 생성 실패");
			return false;
		}

		s_pss[ps->GetName()] = ps;

		return true;
	}

	bool ShaderManager::CreateShaderProgram(const std::string& vs, const std::string& ps, const std::string& name)
	{
		auto vs_it = s_vss.find(vs);
		if (vs_it == s_vss.end())
		{
			return false;
		}

		auto ps_it = s_pss.find(ps);
		if (ps_it == s_pss.end())
		{
			return false;
		}

		s_spCache[name] = std::make_shared<ShaderProgram>(vs_it->second.first, ps_it->second, vs_it->second.second, name);

		return true;
	}
}