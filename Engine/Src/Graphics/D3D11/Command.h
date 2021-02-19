#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;
	struct PipelineState;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class Dive_Texture;

	// 이걸 누가 가져야 할까?
	class Command : public Object
	{
		DIVE_OBJECT(Command, Object);

	public:
		Command(Context* context);
		~Command();

		bool SetPipelineState(const PipelineState& state);

		// 스파르탄은 unordered access view와 depth stencil view도 이 하나의 함수로 초기화했다.
		// 직접 받지 않았다. State를 전달받은 후 초기화했다.
		void ClearRenderTarget(Dive_Texture* texture, DirectX::XMFLOAT4 color);
		// 흐음... 기본 RenderTarget은 이게 맞다.
		void ClearRenderTarget(ID3D11RenderTargetView* rtv, DirectX::XMFLOAT4 color);

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