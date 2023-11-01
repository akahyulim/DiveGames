#pragma once
#include "Resource/Resource.h"
#include "Graphics/GraphicsDefs.h"

namespace Dive
{
	class Texture;

	// 머티리얼이 셰이더를 가지는건 일반적인 구현인듯 하다.
	// 문제는 머티리얼을 누가 가지느냐이다.
	// 기본적으로 메시렌더러와 메시가 1:1 매칭이 되지만 아닐 경우 존재하기 때문이다.
	class Material : public Resource
	{
	public:
		Material();
		~Material() override;

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		const std::unordered_map<eTextureUnit, Texture*>& GetTextures() const { return m_Textures; }
		Texture* GetTexture(eTextureUnit unit) const;
		void SetTexture(eTextureUnit unit, Texture* pTexture);
		void AddTexture(eTextureUnit unit, const std::string& name);

		DirectX::XMFLOAT4 GetDiffuseColor() const { return m_DiffuseColor; }
		void SetDiffuseColor(float r, float g, float b, float a) { SetDiffuseColor(DirectX::XMFLOAT4(r, g, b, a)); }
		void SetDiffuseColor(const DirectX::XMFLOAT4& color) { m_DiffuseColor = color; }

		DirectX::XMFLOAT2 GetTiling() const { return m_Tiling; }
		void SetTiling(DirectX::XMFLOAT2 tiling) { m_Tiling = tiling; }
		void SetTiling(float x, float y) { m_Tiling.x = x; m_Tiling.y = y; }

		DirectX::XMFLOAT2 GetOffset() const { return m_Offset; }
		void SetOffset(DirectX::XMFLOAT2 offset) { m_Offset = offset; }
		void SetOffset(float x, float y) { m_Offset.x = x; m_Offset.y = y; }

	private:
	private:
		std::unordered_map<eTextureUnit, Texture*> m_Textures;
		DirectX::XMFLOAT4 m_DiffuseColor;

		DirectX::XMFLOAT2 m_Tiling;
		DirectX::XMFLOAT2 m_Offset;
		
	};
}