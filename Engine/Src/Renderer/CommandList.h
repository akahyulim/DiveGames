#pragma once

namespace Dive
{
	struct PipelineState;

	class CommandList
	{
	public:

		bool SetPipelineState(PipelineState& ps);

		// sampler
		// viewport
		void SetVertexBuffer(ID3D11Buffer* pVertexBuffer, const UINT strides, const UINT offsets);
		// index buffer
		// constant buffer
		// texture: shader resource view

		// draw

	private:
		PipelineState* m_pPipelineState;
	};
}