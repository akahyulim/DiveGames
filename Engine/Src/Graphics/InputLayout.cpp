#include "divepch.h"
#include "InputLayout.h"
#include "Graphics.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Graphics.h"
#include "GraphicsDefs.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	InputLayout::InputLayout(Context* pContext, VertexBuffer* pBuffers, Shader* pVertexShader)
		: Object(pContext),
		m_pInputLayout(nullptr)
	{
		if (!pBuffers || !pVertexShader || pVertexShader->GetShaderType() != eShaderType::Vertex)
		{
			DV_LOG_ENGINE_ERROR("�߸��� ���ڸ� ���޹޾� InputLayout ������ �����Ͽ����ϴ�.");
			return;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> elementDescs;

		const auto& srcElements = pBuffers->GetElements();

		for (unsigned int i = 0; i < (unsigned int)srcElements.size(); ++i)
		{
			const VertexElement& srcElement = srcElements[i];
			const char* pSemanticName = ELEMENT_SEMANTICNAMES[srcElement.m_Semantic];

			D3D11_INPUT_ELEMENT_DESC desc;
			desc.SemanticName = pSemanticName;	
			desc.SemanticIndex = srcElement.m_Index;
			desc.Format = ELEMENT_FORMATS[srcElement.m_Type];
			desc.InputSlot = 0;	// ���۸� �迭�� �����Ͽ� �� ���� ���� �� �ִ� �� �ϴ�.
			desc.AlignedByteOffset = srcElement.m_Offset;
			desc.InputSlotClass = srcElement.m_PerInstnace ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = srcElement.m_PerInstnace ? 1: 0;

			elementDescs.emplace_back(desc);
		}

		if (elementDescs.empty())
			return;

		const auto& byteCode = pVertexShader->GetByteCode();

		if (FAILED(
			GetSubsystem<Graphics>()->GetDevice()->CreateInputLayout(
			elementDescs.data(),
			(UINT)elementDescs.size(),
			byteCode.data(),
			byteCode.size(),
			(ID3D11InputLayout**)&m_pInputLayout
		)))
		{
			DV_RELEASE(m_pInputLayout);
			DV_LOG_ENGINE_ERROR("InputLayout ������ �����Ͽ����ϴ�.");
		}
	}

	InputLayout::~InputLayout()
	{
		DV_RELEASE(m_pInputLayout);
	}
}