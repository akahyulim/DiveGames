#pragma once
#include "Core/Object.h"

namespace Dive
{
	class ConstantBuffer;
	class Dive_Texture;
	class DepthStencilState;

	class GBuffer : public Object
	{
		DIVE_OBJECT(GBuffer, Object);

	public:
		GBuffer(Context* context);
		~GBuffer();

		bool Initialize(unsigned int width, unsigned int height);
		void Clear();
		 
		void PreRender();
		void PostRender();
		void PrepareForUnpack();
		
		// get
		// color sepc intensity rendertargetview
		// depth stencil view 2개
		// 각각의 shader resource view


		unsigned int GetWidth() const	{ return m_width; }
		unsigned int GetHeight() const	{ return m_height; }

	private:
	private:
		ConstantBuffer* m_constantBuffer;
		
		// texture2d
		Dive_Texture* m_colorSpecIntensity;
		Dive_Texture* m_normal;
		Dive_Texture* m_specPower;
		Dive_Texture* m_dpethStencilReadOnly;
		// depth stencil 2개다. => 일단 하나만...

		// depth stencil을 모두 사용한다. 
		// 그런데 직접 생성하는 것은 에바다... PreRender()에서 전달 받아야 한다.
		DepthStencilState* m_depthStencilState;	

		unsigned int m_width;
		unsigned int m_height;
	};
}