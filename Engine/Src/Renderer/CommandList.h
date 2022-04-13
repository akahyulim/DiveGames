#pragma once
#include "PipelineState.h"
#include "Renderer.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;

	class CommandList
	{
	public:

		bool BindPipelineState(PipelineState& ps);

		// sampler
		// viewport
		void SetVertexBuffer(VertexBuffer* pVertexBuffer, unsigned int offsets = 0);
		void SetIndexBuffer(IndexBuffer* pIndexBuffer, unsigned int offsets = 0);
		void SetConstantBuffer(uint8_t type, eConstantBufferSlot slot, ConstantBuffer* pConstantBuffer);
		// texture: shader resource view

		// draw

	private:
		PipelineState m_OldStates;
	};
}