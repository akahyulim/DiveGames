#include "divepch.h"
#include "Shader.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "IO/FileSystem.h"

namespace Dive
{
	// 현재 매크로는 사용하지 않고 있다.
	ID3DBlob* CompileFromFile(const std::string& fileName, const std::string& profile, const std::string& entryPoint)
	{
		ID3DBlob* pShaderBlob = nullptr;
		ID3DBlob* pErrorMsgs = nullptr;

		std::wstring filePath = StringToWstring(fileName);

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
				DV_CORE_ERROR("{:s}", (char*)pErrorMsgs->GetBufferPointer());
			else
				DV_CORE_ERROR("셰이더 파일({:s}) 컴파일에 실패하였습니다.", fileName);

			DV_RELEASE(pErrorMsgs);
			DV_RELEASE(pShaderBlob);

			return nullptr;
		}

		return pShaderBlob;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements(eVertexLayout layout)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> elements = {
			D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } };

		if (layout == eVertexLayout::Pos_Tex)
		{
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}
		else if (layout == eVertexLayout::Pos_Tex_Nor)
		{
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}
		else if (layout == eVertexLayout::Static_Model)
		{
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}
		else if (layout == eVertexLayout::Skinned_Model)
		{
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}

		return elements;
	}

	Shader::Shader()
		: m_pVertexShaderBlob(nullptr)
		, m_pVertexShader(nullptr)
		, m_pPixelShader(nullptr)
		, m_pInputLayout(nullptr)
	{
	}
	
	Shader::~Shader()
	{
		DV_RELEASE(m_pPixelShader);
		DV_RELEASE(m_pVertexShader);
		DV_RELEASE(m_pVertexShaderBlob);
		DV_RELEASE(m_pInputLayout);

		DV_CORE_TRACE("resource destroy - {0:s}({1:d}), {2:s}({3:d})",
			GetTypeName(), GetTypeHash(), GetName(), GetNameHash());
	}

	// 셰이더 파일이 아니라 inputlayout과 shader file을 정의한 다른 파일일 수 있다.
	bool Shader::LoadFromFile(const std::string& fileName)
	{
		auto pDevice = Graphics::GetDevice();

		m_pVertexShaderBlob = CompileFromFile(fileName, "vs_5_0", "MainVS");
		if (!m_pVertexShaderBlob)
			return false;

		if (FAILED(pDevice->CreateVertexShader(m_pVertexShaderBlob->GetBufferPointer(), m_pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader)))
		{
			DV_CORE_ERROR("버텍스 셰이더 생성에 실패하였습니다.");
			return false;
		}

		auto pPixelShaderBlob = CompileFromFile(fileName, "ps_5_0", "MainPS");
		if (!pPixelShaderBlob)
			return true; // 임시

		if (FAILED(pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &m_pPixelShader)))
		{
			DV_CORE_ERROR("픽셀 셰이더 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool Shader::CreateInputLayout(eVertexLayout layout)
	{
		DV_CORE_ASSERT(m_pVertexShaderBlob);

		auto inputElements = GetInputElements(layout);

		if (FAILED(Graphics::GetDevice()->CreateInputLayout(
			inputElements.data(),
			static_cast<UINT>(inputElements.size()),
			m_pVertexShaderBlob->GetBufferPointer(),
			m_pVertexShaderBlob->GetBufferSize(),
			&m_pInputLayout)))
		{
			DV_CORE_ERROR("InputLayout 생성에 실패하였습니다.");

			return false;
		}

		return true;
	}

	Shader* LoadShaderFromFile(const std::string& fileName)
	{
		Shader* pObject = new Shader;
		if (!pObject->LoadFromFile(fileName))
		{
			DV_DELETE(pObject);
			return nullptr;
		}

		pObject->SetName(fileName);

		return pObject;
	}

	Shader* CreateShader(const std::string& name)
	{
		if (name.empty())
		{
			DV_CORE_ERROR("잘못된 이름({:s})을 전달받았습니다.", name);
			return nullptr;
		}

		Shader* pObject = new Shader;
		pObject->SetName(name);

		return pObject;
	}
}