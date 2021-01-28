#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;
	struct PipelineState;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class Texture2D;

	// �̰� ���� ������ �ұ�?
	class Command : public Object
	{
		DIVE_OBJECT(Command, Object);

	public:
		Command(Context* context);
		~Command();

		bool SetPipelineState(const PipelineState& state);

		// ���ĸ�ź�� unordered access view�� depth stencil view�� �� �ϳ��� �Լ��� �ʱ�ȭ�ߴ�.
		void ClearRenderTarget(Texture2D* texture, DirectX::XMFLOAT4 color);

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