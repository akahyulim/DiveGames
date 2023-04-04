#include "DivePch.h"
#include "InputLayout.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	InputLayout::InputLayout()
		: m_VertexType(eVertexType::Undefined),
		m_pInputLayout(nullptr)
	{
	}
	
	InputLayout::~InputLayout()
	{
		DV_RELEASE(m_pInputLayout);
	}

	bool InputLayout::Create(ID3D10Blob* pShaderBuffer, eVertexType type)
	{
		DV_ASSERT(pShaderBuffer);
		DV_ASSERT(type != eVertexType::Undefined);

		std::vector<D3D11_INPUT_ELEMENT_DESC> elements = {
			D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } };

		if (type == eVertexType::PosCol)
		{
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}
		else if (type == eVertexType::PosTex)
		{
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}
		else if (type == eVertexType::Model)
		{
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			elements.emplace_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}

		auto* pDevice = Graphics::GetDevice();
		DV_ASSERT(pDevice);

		if (FAILED(pDevice->CreateInputLayout(
			elements.data(),
			static_cast<UINT>(elements.size()),
			pShaderBuffer->GetBufferPointer(),
			pShaderBuffer->GetBufferSize(),
			&m_pInputLayout)))
		{
			DV_CORE_ERROR("InputLayout 생성에 실패하였습니다.");
			return false;
		}

		m_VertexType = type;

		return true;
	}
}