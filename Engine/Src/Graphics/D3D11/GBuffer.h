#pragma once
#include "Core/Object.h"

namespace Dive
{
	class ConstantBuffer;
	class Texture2D;
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
		Texture2D* m_colorSpecIntensity;
		Texture2D* m_normal;
		Texture2D* m_specPower;
		// depth stencil 2개다. => 일단 하나만...

		// depth stencil을 모두 사용한다. 
		// 그런데 직접 생성하는 것은 에바다... PreRender()에서 전달 받아야 한다.
		DepthStencilState* m_depthStencilState;	

		unsigned int m_width;
		unsigned int m_height;
	};
}