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

	// shader가 주체이다. 헷갈리지 말자.
	// 여기에서 shader는 하나의 객체라기 보단 pass에 가깝다.
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

		// 생성 및 관리는 ResourceManager가 한다.
		// 사용 에셋에 따라 동적으로 생성되므로 enum이 아닌 포인터로 사용해야 한다.
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