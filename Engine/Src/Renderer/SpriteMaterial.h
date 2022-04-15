#pragma once
#include "Material.h"

namespace Dive
{
	class Texture2D;

	class SpriteMaterial : public Material
	{
	public:
		SpriteMaterial();
		~SpriteMaterial();

		Texture2D* GetSprite() { return m_pSprite; }
		void SetSprite(Texture2D* pSprite) { m_pSprite = pSprite; }
		bool HasSprite() { return m_pSprite != nullptr; }

		DirectX::XMFLOAT4 GetColor() const { return m_Color; }
		void SetColor(const DirectX::XMFLOAT4& color) { m_Color = color; }

		bool IsFlipX() const { return m_bFlipX; }
		void SetFlipX(bool flip) { m_bFlipX = flip; }
		bool IsFlipY() const { return m_bFlipY; }
		void SetFlipY(bool flip) { m_bFlipY = flip; }

	private:
		Texture2D* m_pSprite = nullptr;
		DirectX::XMFLOAT4 m_Color = {1.0f, 1.0f, 1.0f, 1.0f};
		bool m_bFlipX = false;
		bool m_bFlipY = false;
	};
}