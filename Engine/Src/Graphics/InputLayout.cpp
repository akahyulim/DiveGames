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
	InputLayout::InputLayout(Context* pContext, VertexBuffer** ppBuffers, Shader* pVertexShader)
		: Object(pContext),
		m_pInputLayout(nullptr)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> elementDescs;
		unsigned int prevBufferDescs = 0;

		for (int i = 0; i < MAX_VERTEX_STREAMS; ++i)
		{
			if (!ppBuffers[i])
				continue;

			const auto& srcElements = ppBuffers[i]->GetElements();
			bool bExisting = false;

			for (unsigned int j = 0; j < (unsigned int)srcElements.size(); ++j)
			{
				const VertexElement& srcElement = srcElements[j];
				const char* pSemanticName = ELEMENT_SEMANTICNAMES[srcElement.m_Semantic];

				// 이게 뭘 하는 건지 모르겠다.
				for (unsigned int k = 0; k < prevBufferDescs; ++k)
				{
					if (elementDescs[k].SemanticName == pSemanticName && elementDescs[k].SemanticIndex == srcElement.m_Index)
					{
						bExisting = true;
						elementDescs[k].InputSlot = i;
						elementDescs[k].AlignedByteOffset = srcElement.m_Offset;
						elementDescs[k].InputSlotClass = srcElement.m_PerInstnace ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
						elementDescs[k].InstanceDataStepRate = srcElement.m_PerInstnace ? 1 : 0;
						break;
					}
				}

				if (bExisting)
					continue;

				D3D11_INPUT_ELEMENT_DESC desc;
				desc.SemanticName = pSemanticName;
				desc.SemanticIndex = srcElement.m_Index;
				desc.Format = ELEMENT_FORMATS[srcElement.m_Type];
				desc.InputSlot = i;
				desc.AlignedByteOffset = srcElement.m_Offset;
				desc.InputSlotClass = srcElement.m_PerInstnace ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
				desc.InstanceDataStepRate = srcElement.m_PerInstnace ? 1 : 0;

				elementDescs.emplace_back(desc);
			}

			prevBufferDescs = (unsigned int)elementDescs.size();
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
			DV_LOG_ENGINE_ERROR("InputLayout 생성에 실패하였습니다.");
		}
	}

	InputLayout::~InputLayout()
	{
		DV_RELEASE(m_pInputLayout);
	}
}