#include "divepch.h"
#include "InputLayout.h"
#include "Graphics.h"
#include "VertexBuffer.h"
#include "ShaderVariation.h"
#include "Graphics.h"
#include "GraphicsDefs.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	// InputSlot이 2개 이상일 경우를 대응하지 못했다.
	InputLayout::InputLayout(Context* pContext, VertexBuffer* pBuffers, ShaderVariation* pVertexShader)
		: Object(pContext)
	{
		if (!pBuffers || !pVertexShader || pVertexShader->GetShaderType() != eShaderType::Vertex)
		{
			DV_LOG_ENGINE_ERROR("잘못된 인자를 전달받아 InputLayout 생성에 실패하였습니다.");
			return;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> elementDescs;

		const auto& srcElements = pBuffers->GetElements();

		for (unsigned int i = 0; i < static_cast<unsigned int>(srcElements.size()); ++i)
		{
			const VertexElement& srcElement = srcElements[i];
			const char* pSemanticName = ELEMENT_SEMANTICNAMES[srcElement.m_Semantic];

			D3D11_INPUT_ELEMENT_DESC desc;
			desc.SemanticName = pSemanticName;	
			desc.SemanticIndex = srcElement.m_Index;
			desc.Format = ELEMENT_FORMATS[srcElement.m_Type];
			desc.InputSlot = 0;	// texCoord0, texCoord1 이런거다.
			desc.AlignedByteOffset = srcElement.m_Offset;
			desc.InputSlotClass = srcElement.m_PerInstnace ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = srcElement.m_PerInstnace ? 1: 0;

			elementDescs.emplace_back(desc);
		}

		if (elementDescs.empty())
			return;

		auto pShaderBuffer = pVertexShader->GetShaderBuffer();
		if (!pShaderBuffer)
			return;

		auto pGraphics = GetSubsystem<Graphics>();
		if (!pGraphics || !pGraphics->IsInitialized())
			return;

		if (FAILED(pGraphics->GetDevice()->CreateInputLayout(
			elementDescs.data(),
			static_cast<UINT>(elementDescs.size()),
			pShaderBuffer->GetBufferPointer(),
			pShaderBuffer->GetBufferSize(),
			m_pInputLayout.GetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("InputLayout 생성에 실패하였습니다.");
		}
	}
}