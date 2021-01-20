#include "DivePch.h"
#include "Command.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "PipelineState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "Shader.h"
#include "Graphics/Graphics.h"
#include "Core/Context.h"
#include "Core/Log.h"


namespace Dive
{
	Command::Command(Context* context)
		: Object(context)
	{

	}
	
	Command::~Command()
	{
	}

	bool Command::SetPipelineState(const PipelineState & state)
	{
		// invalid 확인

		auto immediateContext = m_context->GetSubsystem<Graphics>()->GetRHIContext();
		if (!immediateContext)
		{
			CORE_ERROR("");
			return false;
		}

		// 기타 처리를 위해 멤버 변수로 저장해놓았다...

		// Input Assembly
		{
			// Input Layout
			{
				ID3D11InputLayout* currentInputLayout = state.vertexShader ? state.vertexShader->GetInputLayout() : nullptr;
				
				ID3D11InputLayout* oldInputLayout = nullptr;
				immediateContext->IAGetInputLayout(&oldInputLayout);

				if (currentInputLayout != oldInputLayout)
					immediateContext->IASetInputLayout(currentInputLayout);
			}

			// Primitive Topology
			{
				auto currentTopology = state.primitiveTopology;

				D3D11_PRIMITIVE_TOPOLOGY oldTopology;
				immediateContext->IAGetPrimitiveTopology(&oldTopology);

				if(currentTopology != oldTopology)
					immediateContext->IASetPrimitiveTopology(currentTopology);
			}
		}

		// Vertex Shader
		{
			// Vertex Shader
		}

		// Rasterizer
		{
			// Rasterizer State
			{
				ID3D11RasterizerState* currentRasterizerState = state.rasterizerState ? state.rasterizerState->GetState() : nullptr;

				ID3D11RasterizerState* oldRasterizerState = nullptr;
				immediateContext->RSGetState(&oldRasterizerState);

				if (currentRasterizerState != oldRasterizerState)
					immediateContext->RSSetState(currentRasterizerState);
			}

			// Vewport
			{
				auto currentViewport = state.viewport;

				D3D11_VIEWPORT oldViewport;
				unsigned int numViewports = 0;
				immediateContext->RSGetViewports(&numViewports, &oldViewport);

				if(currentViewport != oldViewport)
					immediateContext->RSSetViewports(1, &currentViewport);
			}
		}

		// Pixel Shader
		{
			// Pixel Shader
		}

		// Out Merge
		{
			// Blend State
			{
			
			}

			// DepthStencil State
			{
				ID3D11DepthStencilState* currentDepthStencilState = state.depthStencilState ? state.depthStencilState->GetState() : nullptr;
				
				ID3D11DepthStencilState* oldDepthStencilState = nullptr;
				unsigned int stencilRef = 0;
				immediateContext->OMGetDepthStencilState(&oldDepthStencilState, &stencilRef);

				if(currentDepthStencilState != oldDepthStencilState)
					immediateContext->OMSetDepthStencilState(currentDepthStencilState, 1);
			}

			// RenderTargets
			{
				// 좀 복잡하다.
			}
		}

		return true;
	}
	
	void Command::SetVertexBuffer(VertexBuffer * buffer, unsigned int offset)
	{
		if (!buffer || !buffer->GetBuffer())
		{
			CORE_ERROR("");
			return;
		}

		// 에바?
		auto immediateContext = m_context->GetSubsystem<Graphics>()->GetRHIContext();
		if (!immediateContext)
		{
			CORE_ERROR("");
			return;
		}

		auto vertexBuffer = buffer->GetBuffer();
		auto strides = buffer->GetStride();
		auto offsets = offset;
		
		

		// 이전 버퍼와 비교

		immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
	}

	void Command::SetIndexBuffer(IndexBuffer * buffer, unsigned int offset)
	{
		if (!buffer || !buffer->GetBuffer())
		{
			CORE_ERROR("");
			return;
		}

		auto immediateContext = m_context->GetSubsystem<Graphics>()->GetRHIContext();
		if (!immediateContext)
		{
			CORE_ERROR("");
			return;
		}

		auto indexBuffer = buffer->GetBuffer();
		auto format = buffer->GetFormat();

		// 이전 버퍼와 비교

		immediateContext->IASetIndexBuffer(indexBuffer, format, offset);
	}
	
	bool Command::Draw(unsigned int vertexCount)
	{
		auto immediateContext = m_context->GetSubsystem<Graphics>()->GetRHIContext();
		if (!immediateContext)
		{
			CORE_ERROR("");
			return false;
		}

		immediateContext->Draw(vertexCount, 0);

		return true;
	}
	
	bool Command::DrawIndexed(unsigned int indexCount, unsigned int indexOffset, unsigned int vertexOffset)
	{
		auto immediateContext = m_context->GetSubsystem<Graphics>()->GetRHIContext();
		if (!immediateContext)
		{
			CORE_ERROR("");
			return false;
		}

		immediateContext->DrawIndexed(indexCount, indexOffset, vertexOffset);

		return true;
	}
}