#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;

	class Sampler : public Object
	{
		DIVE_OBJECT(Sampler, Object);

	public:
		Sampler(Context* context,
			const D3D11_FILTER textureFilter,
			const D3D11_TEXTURE_ADDRESS_MODE addressMode	= D3D11_TEXTURE_ADDRESS_WRAP,
			const D3D11_COMPARISON_FUNC cmpFunc				= D3D11_COMPARISON_ALWAYS,
			UINT maxAnisotropy								= 2);
		~Sampler();

		ID3D11SamplerState* GetState() const { return m_state; }
	
	private:
		ID3D11SamplerState* m_state;
	};
}