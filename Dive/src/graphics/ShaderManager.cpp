#include "stdafx.h"
#include "ShaderManager.h"
#include "Graphics.h"

namespace Dive
{
	std::unordered_map<std::string, std::unique_ptr<IShader>> ShaderManager::s_shaders;
	std::unordered_map<InputLayoutKey, Microsoft::WRL::ComPtr<ID3D11InputLayout>> ShaderManager::s_inputLayouts;

	static std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements(eInputLayout layout)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> elements;

		switch (layout)
		{
		case eInputLayout::PN:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		case eInputLayout::PNT:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		case eInputLayout::Unlit:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		case eInputLayout::Lit:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		case eInputLayout::Skinned:
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
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
		// vertex shader & inputLayout 
		{
			// unlit
			auto unlit_vs = std::make_unique<VertexShader>();
			if (!unlit_vs->LoadFromFile("Assets/Shaders/UnlitVS.cso"))
			{
				DV_LOG(ShaderManager, err, "[::Initialize] LoadFromFile 실패");
				return false;
			}

			if (!CreateInputLayout(unlit_vs.get(), eInputLayout::Unlit))
			{
				DV_LOG(ShaderManager, err, "[::Initialize] CreateInputLayout 실패");
				return false;
			}

			auto name = unlit_vs->GetName();
			auto raw_ptr = unlit_vs.release();
			s_shaders.emplace(name, std::unique_ptr<IShader>(raw_ptr));

			// forward light
			auto forwardLight_vs = std::make_unique<VertexShader>();
			if (!forwardLight_vs->LoadFromFile("Assets/Shaders/ForwardLightVS.cso"))
			{
				DV_LOG(ShaderManager, err, "[::Initialize] LoadFromFile 실패");
				return false;
			}

			if (!CreateInputLayout(forwardLight_vs.get(), eInputLayout::Lit))
			{
				DV_LOG(ShaderManager, err, "[::Initialize] CreateInputLayout 실패");
				return false;
			}

			name = forwardLight_vs->GetName();
			raw_ptr = forwardLight_vs.release();
			s_shaders.emplace(name, std::unique_ptr<IShader>(raw_ptr));
		}

		// pixel shaders
		{
			// unlit
			auto unlit_ps = std::make_unique<PixelShader>();
			if (!unlit_ps->LoadFromFile("Assets/Shaders/UnlitPS.cso"))
			{
				DV_LOG(ShaderManager, err, "[::Initialize] LoadFromFile 실패");
				return false;
			}

			auto name = unlit_ps->GetName();
			auto raw_ptr = unlit_ps.release();
			s_shaders.emplace(name, std::unique_ptr<IShader>(raw_ptr));

			// forward light
			auto forwardLight_ps = std::make_unique<PixelShader>();
			if (!forwardLight_ps->LoadFromFile("Assets/Shaders/ForwardLightPS.cso"))
			{
				DV_LOG(ShaderManager, err, "[::Initialize] LoadFromFile 실패");
				return false;
			}

			name = forwardLight_ps->GetName();
			raw_ptr = forwardLight_ps.release();
			s_shaders.emplace(name, std::unique_ptr<IShader>(raw_ptr));
		}

		// shader programs

		DV_LOG(ShaderManager, info, "초기화 성공");

		return true;
	}

	void ShaderManager::Clear()
	{
		for (auto& [inputLayoutKey, inputLayout] : s_inputLayouts)
			inputLayout.Reset();
		s_inputLayouts.clear();

		for (auto& [name, shader] : s_shaders)
			shader.reset();
		s_shaders.clear();

		DV_LOG(ShaderManager, info, "클리어 완료");
	}

	bool ShaderManager::CreateInputLayout(const VertexShader* vs, eInputLayout type)
	{
		assert(vs && vs->GetBytecode());

		auto inputElements = GetInputElements(type);
		auto byteCode = vs->GetBytecode();
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

		auto hr = Graphics::GetDevice()->CreateInputLayout(
			inputElements.data(),
			static_cast<UINT>(inputElements.size()),
			byteCode->GetBufferPointer(),
			byteCode->GetBufferSize(),
			inputLayout.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(Shader, err, "[::InputLayout] 생성 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		s_inputLayouts.emplace(InputLayoutKey(vs->GetName(), type), inputLayout);

		return true;
	}

	IShader* ShaderManager::GetShader(const std::string& name)
	{
		auto it = s_shaders.find(name);
		assert(it != s_shaders.end());
		return it->second.get();
	}

	ID3D11InputLayout* ShaderManager::GetInputLayout(const std::string& shaderName, eInputLayout inputLayoutType)
	{
		auto it = s_inputLayouts.find(InputLayoutKey(shaderName, inputLayoutType));
		assert(it != s_inputLayouts.end());
		return it->second.Get();
	}

	void ShaderManager::BindInputLayout(const std::string& shaderName, eInputLayout inputLayoutType)
	{
		Graphics::GetDeviceContext()->IASetInputLayout(s_inputLayouts[InputLayoutKey(shaderName, inputLayoutType)].Get());
	}
}