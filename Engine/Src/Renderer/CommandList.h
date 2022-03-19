#pragma once

namespace Dive
{
	struct PipelineState;

	class CommandList
	{
	public:

		// state
		// bind pipeliestate	
		bool SetPipelineState(PipelineState& ps);

		// asset data
		// sampler
		// vertex buffer
		// index buffer
		// constant buffer
		// texture: shader resource view

		// draw

	public:
	private:
		PipelineState* m_pPipelineState;
	};
}