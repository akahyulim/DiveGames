#pragma once

namespace Dive
{
	class Texture;

	enum class eMaterialMapType
	{
		Albedo = 0,
		Metalic,
		Normal,
		Occlusion,
		Edmision,
		DetailMask,
		Count
	};

	// ����Ƽ�� ��� �������� ������ Material�� �����Ѵ�.
	// �̰͵� Resource ���
	class Material
	{
	public:
		Material();
		~Material() = default;

		// ���ĸ�ź�� ��� xml�� ����ȭ�Ѵ�.

		Texture* GetMap(const eMaterialMapType type);
		void SetMap(const eMaterialMapType type, const Texture* pTexture);

		const DirectX::XMFLOAT4 GetAlbedoColor() const { return m_AlbedoColor; }
		void SetAlbedoColor(const DirectX::XMFLOAT4& color) { m_AlbedoColor = color; }

		const DirectX::XMFLOAT2 GetTiling() const { return m_Tiling; }
		void SetTiling(const DirectX::XMFLOAT2& tiling) { m_Tiling = tiling; }

		const DirectX::XMFLOAT2 GetOffset() const { return m_Offset; }
		void SetOffset(const DirectX::XMFLOAT2& offset) { m_Offset = offset; }

	private:

	private:
		// shader
		// rendering mode

		// Main Maps
		std::unordered_map<eMaterialMapType, Texture*> m_MainMaps;
		DirectX::XMFLOAT4 m_AlbedoColor = { 1.0f, 1.0f ,1.0f, 1.0f };
		DirectX::XMFLOAT2 m_Tiling		= { 1.0f, 1.0f };
		DirectX::XMFLOAT2 m_Offset		= { 0.0f, 0.0f };

		// Second Maps
	};
}