#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class VertexBuffer;
	class Shader;

	class InputLayout : public Object
	{
		DIVE_OBJECT(InputLayout, Object)

	public:
		explicit InputLayout(Context* pContext, VertexBuffer* pBuffers, Shader* pVertexShader);
		~InputLayout() override = default;

		ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
	};
}