#include "divepch.h"
#include "DvShader.h"
#include "GraphicsDevice.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	// 현재 매크로는 사용하지 않고 있다.
	ID3DBlob* CompileFromFile(const std::wstring& filepath, const std::string& profile, const std::string& entryPoint)
	{
		ID3DBlob* pShaderBlob = nullptr;
		ID3DBlob* pErrorMsgs = nullptr;

		if (FAILED(D3DCompileFromFile(
			filepath.c_str(),
			nullptr, //macros.data(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint.c_str(),
			profile.c_str(),
			D3D10_SHADER_ENABLE_STRICTNESS,
			0,
			&pShaderBlob,
			&pErrorMsgs)))
		{
			//DV_CORE_ERROR("셰이더 파일({:s}) 컴파일에 실패하였습니다.", filepath);
			if (pErrorMsgs)
				DV_CORE_ERROR("{:s}", (char*)pErrorMsgs->GetBufferPointer());
			
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

	DvShader::DvShader(const std::string& name, const std::wstring& filepath, eVertexLayout layout)
		: m_Name(name)
		, m_FilePath(filepath)
		, m_pVertexShaderBlob(nullptr)
		, m_pVertexShader(nullptr)
		, m_pPixelShader(nullptr)
		, m_VertexLayout(layout)
		, m_pInputLayout(nullptr)
	{
		loadFromFile(filepath);
		createInputLayout();
	}
	
	DvShader::~DvShader()
	{
		DV_RELEASE(m_pPixelShader);
		DV_RELEASE(m_pVertexShader);
		DV_RELEASE(m_pVertexShaderBlob);
		DV_RELEASE(m_pInputLayout);
	}

	void DvShader::loadFromFile(const std::wstring& filepath)
	{
		auto pDevice = GraphicsDevice::GetDevice();

		m_pVertexShaderBlob = CompileFromFile(filepath, "vs_5_0", "MainVS");
		pDevice->CreateVertexShader(m_pVertexShaderBlob->GetBufferPointer(), m_pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader);

		auto pPixelShaderBlob = CompileFromFile(filepath, "ps_5_0", "MainPS");
		pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &m_pPixelShader);
	}

	void DvShader::createInputLayout()
	{
		DV_CORE_ASSERT(m_pVertexShaderBlob);

		auto inputElements = GetInputElements(m_VertexLayout);

		if (FAILED(GraphicsDevice::GetDevice()->CreateInputLayout(
			inputElements.data(),
			static_cast<UINT>(inputElements.size()),
			m_pVertexShaderBlob->GetBufferPointer(),
			m_pVertexShaderBlob->GetBufferSize(),
			&m_pInputLayout)))
			DV_CORE_ERROR("InputLayout 생성에 실패하였습니다.");
	}
	
	DvShader* DvShader::CreateFromFile(const std::string& name, const std::wstring& filepath, eVertexLayout layout)
	{
		return new DvShader(name, filepath, layout);
	}
}