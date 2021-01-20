#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;
	struct PipelineState;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;

	// 이걸 누가 가져야 할까?
	class Command : public Object
	{
	public:
		Command(Context* context);
		~Command();

		bool SetPipelineState(const PipelineState& state);

		// buffers
		void SetVertexBuffer(VertexBuffer* buffer, unsigned int offset = 0);
		void SetIndexBuffer(IndexBuffer* buffer, unsigned int offset = 0);

		// draws
		bool Draw(unsigned int vertexCount);
		bool DrawIndexed(unsigned int indexCount, unsigned int indexOffset = 0, unsigned int vertexOffset = 0);

	private:
		Command(const Command&)				= delete;
		Command& operator=(const Command&)	= delete;

	private:

	};
}