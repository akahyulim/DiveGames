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
		// depth stencil view 2��
		// ������ shader resource view


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
		// depth stencil 2����. => �ϴ� �ϳ���...

		// depth stencil�� ��� ����Ѵ�. 
		// �׷��� ���� �����ϴ� ���� ���ٴ�... PreRender()���� ���� �޾ƾ� �Ѵ�.
		DepthStencilState* m_depthStencilState;	

		unsigned int m_width;
		unsigned int m_height;
	};
}