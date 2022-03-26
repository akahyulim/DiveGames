#pragma once
#include "PipelineState.h"

namespace Dive
{
	class CommandList
	{
	public:

		bool BindPipelineState(PipelineState& ps);

		// sampler
		// viewport
		void SetVertexBuffer(ID3D11Buffer* pVertexBuffer, const UINT strides, const UINT offsets);
		// index buffer
		// constant buffer
		// texture: shader resource view

		// draw

	private:
		PipelineState m_OldStates;
	};
}