#include "Graphics/ShaderManager.h"
#include "Graphics/Graphics.h"
#include "Core/Common.h"
#include <d3dcompiler.h>
#include <spdlog/spdlog.h>

namespace Dive
{
	ShaderManager::~ShaderManager() = default;

	bool ShaderManager::Initialize(Graphics* graphics)
	{
		assert(graphics);

		{
			ShaderProgramDesc desc{};
			desc.name = "GBuffer";
			desc.vsPath = "src/Shaders/GBuffer.hlsl";
			desc.psPath = "src/Shaders/GBuffer.hlsl";
			desc.ilType = eInputLayout::Lit;

			if (!CreateShaderProgram(graphics, desc))
				return false;
		}

		{
			ShaderProgramDesc desc{};
			desc.name = "GBufferTessellation";
			desc.vsPath = "src/Shaders/GBufferTessellation.hlsl";
			desc.psPath = "src/Shaders/GBufferTessellation.hlsl";
			desc.hsPath = "src/Shaders/GBufferTessellation.hlsl";
			desc.dsPath = "src/Shaders/GBufferTessellation.hlsl";
			desc.ilType = eInputLayout::Lit;
			
			if (!CreateShaderProgram(graphics, desc))
				return false;
		}

		{
			ShaderProgramDesc desc{};
			desc.name = "DeferredLigting";
			desc.vsPath = "src/Shaders/DeferredLigting.hlsl";
			desc.psPath = "src/Shaders/DeferredLigting.hlsl";
			desc.ilType = eInputLayout::None;

			if (!CreateShaderProgram(graphics, desc))
				return false;
		}

		{
			ShaderProgramDesc desc{};
			desc.name = "Forward";
			desc.vsPath = "src/Shaders/Forward.hlsl";
			desc.psPath = "src/Shaders/Forward.hlsl";
			desc.ilType = eInputLayout::Lit;

			if (!CreateShaderProgram(graphics, desc))
				return false;
		}

		{
			ShaderProgramDesc desc{};
			desc.name = "ForwardTessellation";
			// 미구현
		}

		{
			ShaderProgramDesc desc{};
			desc.name = "Skybox";
			desc.vsPath = "src/Shaders/Skybox.hlsl";
			desc.psPath = "src/Shaders/Skybox.hlsl";
			desc.ilType = eInputLayout::Position;

			if (!CreateShaderProgram(graphics, desc))
				return false;
		}

		{
			ShaderProgramDesc desc{};
			desc.name = "SkySphere";
			desc.vsPath = "src/Shaders/SkySphere.hlsl";
			desc.psPath = "src/Shaders/SkySphere.hlsl";
			desc.ilType = eInputLayout::Position;

			if (!CreateShaderProgram(graphics, desc))
				return false;
		}

		{
			ShaderProgramDesc desc{};
			desc.name = "UniformSky";
			desc.vsPath = "src/Shaders/SkySphere.hlsl";
			desc.psPath = "src/Shaders/UniformSky.hlsl";
			desc.ilType = eInputLayout::Position;

			if (!CreateShaderProgram(graphics, desc))
				return false;
		}

		{
			ShaderProgramDesc desc{};
			desc.name = "Resolve";
			desc.vsPath = "src/Shaders/ResolveScene.hlsl";
			desc.psPath = "src/Shaders/ResolveScene.hlsl";
			desc.ilType = eInputLayout::None;

			if (!CreateShaderProgram(graphics, desc))
				return false;
		}

		auto pickerCS = createShaderInternal(graphics, "src/Shaders/Picker.hlsl", eShaderStage::CS, m_csMap);
		if (!pickerCS)
			return false;
		/*
		if (!createVertexShaderAndInputLayout(graphics, "Source/Shaders/GBuffer.hlsl", eInputLayout::Lit))
		{
			spdlog::error("GBuffer VS 생성 실패");
			return false;
		}
		if (!createVertexShaderAndInputLayout(graphics, "Source/Shaders/GBufferTessellation.hlsl", eInputLayout::Lit))
		{
			spdlog::error("GBufferTessellation VS 생성 실패");
			return false;
		}
		if (!createVertexShaderAndInputLayout(graphics, "Source/Shaders/DeferredLighting.hlsl", eInputLayout::None))
		{
			spdlog::error("DeferredLighting VS 생성 실패");
			return false;
		}
		if (!createVertexShaderAndInputLayout(graphics, "Source/Shaders/Forward.hlsl", eInputLayout::Lit))
		{
			spdlog::error("Forward VS 생성 실패");
			return false;
		}
		if (!createVertexShaderAndInputLayout(graphics, "Source/Shaders/Skybox.hlsl", eInputLayout::Position))
		{
			spdlog::error("Skybox VS 생성 실패");
			return false;
		}
		if (!createVertexShaderAndInputLayout(graphics, "Source/Shaders/SkySphere.hlsl", eInputLayout::Position))
		{
			spdlog::error("SkySphere VS 생성 실패");
			return false;
		}
		if (!createVertexShaderAndInputLayout(graphics, "Source/Shaders/ResolveScene.hlsl", eInputLayout::None))
		{
			spdlog::error("ResolveScene VS 생성 실패");
			return false;
		}

		// hull shader
		if (!createHullShader(graphics, "Source/Shaders/GBufferTessellation.hlsl"))
		{
			spdlog::error("GBufferTessellation HS 생성 실패");
			return false;
		}

		// domain shader
		if (!createDomainShader(graphics, "Source/Shaders/GBufferTessellation.hlsl"))
		{
			spdlog::error("GBufferTessellation DS 생성 실패");
			return false;
		}

		// pixel shader
		if(!createPixelShader(graphics, "Source/Shaders/GBuffer.hlsl"))
		{
			spdlog::error("GBuffer PS 생성 실패");
			return false;
		}
		if (!createPixelShader(graphics, "Source/Shaders/GBufferTessellation.hlsl"))
		{
			spdlog::error("GBufferTessellation PS 생성 실패");
			return false;
		}
		if (!createPixelShader(graphics, "Source/Shaders/DeferredLighting.hlsl"))
		{
			spdlog::error("DeferredLighting PS 생성 실패");
			return false;
		}
		if (!createPixelShader(graphics, "Source/Shaders/Forward.hlsl"))
		{
			spdlog::error("Forward PS 생성 실패");
			return false;
		}
		if (!createPixelShader(graphics, "Source/Shaders/Skybox.hlsl"))
		{
			spdlog::error("Skybox PS 생성 실패");
			return false;
		}
		if (!createPixelShader(graphics, "Source/Shaders/SkySphere.hlsl"))
		{
			spdlog::error("SkySphere PS 생성 실패");
			return false;
		}
		if (!createPixelShader(graphics, "Source/Shaders/UniformSky.hlsl"))
		{
			spdlog::error("UniformSky PS 생성 실패");
			return false;
		}
		if (!createPixelShader(graphics, "Source/Shaders/ResolveScene.hlsl"))
		{
			spdlog::error("ResolveScene PS 생성 실패");
			return false;
		}

		// computeShader
		if (!createComputeShader(graphics, "Source/Shaders/Picker.hlsl"))
		{
			spdlog::error("Picking CS 생성 실패");
			return false;
		}

		// shader program
		if(!createShaderProgram("GBuffer", "GBuffer", eShaderPrograms::GBuffer))
		{
			spdlog::error("GBuffer ShaderProgram 생성 실패");
			return false;
		}
		{
			if (!createShaderProgram("GBufferTessellation", "GBufferTessellation", eShaderPrograms::GBufferTessellation))
			{
				spdlog::error("GBufferTessellation ShaderProgram 생성 실패");
				return false;
			}

			m_shaderProgramMap[eShaderPrograms::GBufferTessellation]->SetHullShader(m_hsMap.find("GBufferTessellation")->second.get());
			m_shaderProgramMap[eShaderPrograms::GBufferTessellation]->SetDomainShader(m_dsMap.find("GBufferTessellation")->second.get());
		}
		if (!createShaderProgram("DeferredLighting", "DeferredLighting", eShaderPrograms::DeferredLighting))
		{
			spdlog::error("DeferredLighting ShaderProgram 생성 실패");
			return false;
		}
		if (!createShaderProgram("Forward", "Forward", eShaderPrograms::Forward))
		{
			spdlog::error("Forward ShaderProgram 생성 실패");
			return false;
		}
		// 하나의 hlsl파일에 vs, ps를 모두 구현하면 이렇게 동일한 이름으로 저장된다.
		// enum class로 미리 선언해놓는 것도 하나의 방법이다.
		if (!createShaderProgram("Skybox", "Skybox", eShaderPrograms::Skybox))
		{
			spdlog::error("Skybox ShaderProgram 생성 실패");
			return false;
		}
		if (!createShaderProgram("SkySphere", "SkySphere", eShaderPrograms::SkySphere))
		{
			spdlog::error("SkySphere ShaderProgram 생성 실패");
			return false;
		}
		if (!createShaderProgram("SkySphere", "UniformSky", eShaderPrograms::UniformSky))
		{
			spdlog::error("UniformSky ShaderProgram 생성 실패");
			return false;
		}
		if (!createShaderProgram("ResolveScene", "ResolveScene", eShaderPrograms::Resolve))
		{
			spdlog::error("ResloveScene ShaderProgram 생성 실패");
			return false;
		}
		
		m_shaderProgramMap[eShaderPrograms::Picking] = std::make_unique<ShaderProgram>(m_csMap.find("Picker")->second.get());

		spdlog::info("ShaderManager 초기화 완료");
		*/

		spdlog::info("ShaderManager 초기화 완료");

		return true;
	}

	void ShaderManager::Shutdown()
	{
		m_shaderProgramMap.clear();
		m_ilMap.clear();
		m_psMap.clear();
		m_vsMap.clear();
	}

	ShaderProgram* ShaderManager::CreateShaderProgram(Graphics* graphics, ShaderProgramDesc& desc)
	{
		assert(!desc.name.empty());
		auto it = m_shaderProgramMap.find(desc.name);
		if (it != m_shaderProgramMap.end())
		{
			spdlog::warn("동일한 이름의 셰이더 프로그램이 이미 존재: {}", desc.name);
			return nullptr;
		}

		assert(!desc.vsPath.empty());

		if (m_vsMap.find(desc.vsPath) == m_vsMap.end())
		{
			createVertexShaderAndInputLayout(graphics, desc.vsPath, desc.ilType);
		}

		auto vs = m_vsMap[desc.vsPath].get();

		InputLayout* il = nullptr;
		if (desc.ilType != eInputLayout::None)
		{
			auto ilIt = m_ilMap.find({ desc.vsPath, desc.ilType });
			if (ilIt != m_ilMap.end())
				il = ilIt->second.get();
		}

		assert(!desc.psPath.empty());
		auto ps = createShaderInternal<PixelShader>(graphics, desc.psPath, eShaderStage::PS, m_psMap);
		
		m_shaderProgramMap[desc.name] = std::make_unique<ShaderProgram>(vs, ps, il);

		if (!desc.dsPath.empty())
		{
			auto ds = createShaderInternal<DomainShader>(graphics, desc.dsPath, eShaderStage::DS, m_dsMap);
			m_shaderProgramMap[desc.name]->SetDomainShader(ds);
		}

		if (!desc.hsPath.empty())
		{
			auto hs = createShaderInternal<HullShader>(graphics, desc.hsPath, eShaderStage::HS, m_hsMap);
			m_shaderProgramMap[desc.name]->SetHullShader(hs);
		}
	
		return m_shaderProgramMap[desc.name].get();
	}

	ShaderProgram* ShaderManager::GetShaderProgram(const std::string name)
	{
		auto it = m_shaderProgramMap.find(name);
		if (it != m_shaderProgramMap.end())
			return it->second.get();

		return nullptr;
	}

	ComputeShader* ShaderManager::CreateComputeShader(Graphics* graphics, const std::filesystem::path& path)
	{
		return createShaderInternal(graphics, path, eShaderStage::CS, m_csMap);
	}

	ComputeShader* ShaderManager::GetComputeShader(const std::filesystem::path& path)
	{
		auto it = m_csMap.find(path);
		return it != m_csMap.end() ? it->second.get() : nullptr;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> ShaderManager::compileShader(const std::filesystem::path& filepath, eShaderStage stage)
	{
		if (!std::filesystem::exists(filepath))
		{
			spdlog::error("셰이더 파일을 찾을 수 없습니다: {}", filepath.string());
			return nullptr;
		}

		Microsoft::WRL::ComPtr<ID3DBlob> blob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

		DWORD compileFlags = 0;
#if _DEBUG
		compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_PREFER_FLOW_CONTROL;
#else
		compileFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		std::string entryPoint, target;
		switch (stage)
		{
		case eShaderStage::VS:
		{
			entryPoint = "MainVS";
			target = "vs_5_0";
			break;
		}
		case eShaderStage::HS:
		{
			entryPoint = "MainHS";
			target = "hs_5_0";
			break;
		}case eShaderStage::DS:
		{
			entryPoint = "MainDS";
			target = "ds_5_0";
			break;
		}case eShaderStage::PS:
		{
			entryPoint = "MainPS";
			target = "ps_5_0";
			break;
		}
		case eShaderStage::CS:
		{
			entryPoint = "MainCS";
			target = "cs_5_0";
			break;
		}
		default:
			spdlog::error("지원하지 않는 셰이더 스테이지입니다.");
			return nullptr;
		}

		HRESULT hr = D3DCompileFromFile(
			filepath.wstring().c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint.c_str(),
			target.c_str(),
			compileFlags,
			0,
			blob.GetAddressOf(),
			errorBlob.GetAddressOf()
		);

		if (FAILED(hr))
		{
			if (errorBlob != nullptr)
			{
				spdlog::error("셰이더 컴파일 실패 [{}]: {}",
					filepath.string(),
					(char*)errorBlob->GetBufferPointer());
			}
			else
			{
				spdlog::error("셰이더 파일 컴파일 중 알 수 없는 에러 발생 [{}], HRESULT: 0x{:08X}",
					filepath.string(), static_cast<unsigned int>(hr));
			}
			return nullptr;
		}

		return blob;
	}

	VertexShader* ShaderManager::createVertexShaderAndInputLayout(Graphics* graphics, const std::filesystem::path& path, eInputLayout type)
	{
		if (path.empty())
			return nullptr;

		auto it = m_vsMap.find(path);

		if (it != m_vsMap.end())
		{
			if (type == eInputLayout::None)
				return it->second.get();

			auto key = std::make_pair(path, type);
			if (m_ilMap.find(key) != m_ilMap.end())
			{
				return it->second.get();
			}

			// [주의] 만약 VS는 이미 로드되었는데 새로운 InputLayout 타입이 들어온 경우라면, 
			// 이 시점에서는 blob이 없어서 새로 만들 수 없습니다. 
			// 하지만 보통 하나의 VS 파일은 정해진 하나의 InputLayout과 1:1 매칭되므로 
			// 이 경우는 거의 일어나지 않거나, 애초에 최초 로드 시점에 둘 다 세트로 만들어집니다.
			return it->second.get();
		}

		auto blob = compileShader(path, eShaderStage::VS);
		if (!blob)
			return nullptr;

		m_vsMap[path] = std::make_unique<VertexShader>(graphics, blob.Get());

		if (type != eInputLayout::None)
		{
			auto key = std::make_pair(path, type);
			m_ilMap[key] = std::make_unique<InputLayout>(graphics, type, blob.Get());
		}

		return m_vsMap[path].get();
	}
	/*
	bool ShaderManager::createShaderProgram(const std::string& vsName, const std::string& psName, eShaderPrograms sp)
	{
		VertexShader* vs = nullptr;
		{
			auto it = m_vsMap.find(vsName);
			if (it != m_vsMap.end())
				vs = it->second.get();
			else
				return false;
		}

		PixelShader* ps = nullptr;
		{
			auto it = m_psMap.find(psName);
			if (it != m_psMap.end())
				ps = it->second.get();
			else
				return false;
		}

		InputLayout* il = nullptr;
		{
			auto it = m_ilMap.find(vsName);
			if (it != m_ilMap.end())
				il = it->second.get();
		}

		m_shaderProgramMap[sp] = std::make_unique<ShaderProgram>(vs, ps, il);

		return true;
	}*/
}