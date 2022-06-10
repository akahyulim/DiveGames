#pragma once
#include "DvTexture.h"

namespace Dive
{
	class DvTexture2D : public DvTexture
	{
		RESOURCE_CLASS_TYPE(Texture2D)

	public:
		DvTexture2D();
		~DvTexture2D();

		void Release() override;

		// ���� �Ӹʰ� ��Ƽ���ø��� ������.
		// �ϴ� ��Ƽ���ø��� �����´� �ص� �Ӹ� ������ ���� ���θ� ��� �������� �����ؾ� �Ѵ�.
		bool SetSize(uint32_t width, uint32_t height, DXGI_FORMAT format, eTextureUsage usage = eTextureUsage::TEXTURE_STATIC);
		// SetData: Map 
		// SetData: Image => �ᱹ �̰� Load�� ����ϴ� �Ŵ�...
		// => �����͸� �ε��� �� SetSize�� ����� �ٽ� SetData�� �籸���ϴ� �� �ϴ�.

		// Image ���� resource��.
		// Resource::Load<T>()�� ������ �д´�.
		// ResourceCache(manager?)::GetResource() -> Texture2D::BeginLoad() -> Image::Load() ������ ȣ��ȴ�.
		// Texture2D::EndLoad()���� SetData()�� ������ Image�� �����Ͽ� �����ȴ�.
		// BeginLoad(), EndLoad()�� Resource::Load() ���ο��� ���������� ����ȴ�.


	protected:
		bool Create() override;

	private:
	private:
	};
}