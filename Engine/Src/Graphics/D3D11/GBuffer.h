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
		// depth stencil view 2��
		// ������ shader resource view
		unsigned int GetWidth() const	{ return m_width; }
		unsigned int GetHeight() const	{ return m_height; }

	private:
	private:
		ConstantBuffer* m_constantBuffer;
		
		// texture2d
		Texture2D* m_colorSpecIntensity;
		Texture2D* m_normal;
		Texture2D* m_specPower;
		// depth stencil 2����. => �ϴ� �ϳ���...

		// depth stencil�� ��� ����Ѵ�. 
		// �׷��� ���� �����ϴ� ���� ���ٴ�... PreRender()���� ���� �޾ƾ� �Ѵ�.
		DepthStencilState* m_depthStencilState;	

		unsigned int m_width;
		unsigned int m_height;
	};
}