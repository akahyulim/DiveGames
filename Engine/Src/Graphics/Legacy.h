#pragma once
#include "Material.h"

namespace Dive
{
	class Texture2D;
	class ConstantBuffer;

	enum class eRenderingMode
	{
		Opaque,
		Cutout,
		Transparent,
		Fade
	};

	// shader�� ��ü�̴�. �򰥸��� ����.
	// ���⿡�� shader�� �ϳ��� ��ü��� ���� pass�� ������.
	class Legacy : public Material
	{
		DIVE_OBJECT(Legacy, Material);

	public:
		Legacy(Context* context);
		~Legacy();

		//bool SaveToFile(const std::string& filePath) override;
		//bool LoadFromFile(const std::string& filePath) override;

		ConstantBuffer* GetConstantBuffer();

		// get & set
		eRenderingMode GetRenderingMode() const			{ return m_renderingMode; }
		void SetRenderingMode(eRenderingMode mode);

		Texture2D* GetAlbedoTexture() const				{ return m_albedoTexture; }
		void SetAlbedoTexture(Texture2D* texture);

		DirectX::XMFLOAT4 GetAlbedoColor() const		{ return m_albedoColor; }
		void SetAlbedoColor(DirectX::XMFLOAT4 color);

		DirectX::XMFLOAT2 GetTiling() const				{ return m_tiling; }
		void SetTiling(DirectX::XMFLOAT2 tiling);

		DirectX::XMFLOAT2 GetOffset() const				{ return m_offset; }
		void SetOffset(DirectX::XMFLOAT2 offset);

	private:

	private:
		eRenderingMode m_renderingMode;

		// ���� �� ������ ResourceManager�� �Ѵ�.
		// ��� ���¿� ���� �������� �����ǹǷ� enum�� �ƴ� �����ͷ� ����ؾ� �Ѵ�.
		Texture2D* m_albedoTexture;

		DirectX::XMFLOAT4 m_albedoColor;

		DirectX::XMFLOAT2 m_tiling;
		DirectX::XMFLOAT2 m_offset;

		struct LegacyBuffer
		{
			DirectX::XMFLOAT4 albedoColor;
			DirectX::XMFLOAT2 tiling;
			DirectX::XMFLOAT2 offset;
		};
	};
}