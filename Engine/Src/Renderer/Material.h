#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Texture2D;

	enum class eMaterialMapType
	{
		Albedo = 0,
		Normal,
		Metalic,
		Occlusion,
		Edmision,
		DetailMask,
		Count
	};

	class Material : public Resource
	{
	public:
		RESOURCE_CLASS_TYPE(Material)
	public:
		Material();
		Material(unsigned long long id);
		//Material(const std::string& name = "", unsigned long long id = 0);
		virtual ~Material();

		Texture2D* GetMap(const eMaterialMapType type);
		void SetMap(const eMaterialMapType type, const Texture2D* pTexture);
		bool HasMap(const eMaterialMapType type) { return m_MainMaps[type] != nullptr; }

		const DirectX::XMFLOAT4 GetAlbedoColor() const { return m_AlbedoColor; }
		void SetAlbedoColor(const DirectX::XMFLOAT4& color) { m_AlbedoColor = color; }

		const DirectX::XMFLOAT2 GetTiling() const { return m_Tiling; }
		void SetTiling(const DirectX::XMFLOAT2& tiling) { m_Tiling = tiling; }

		const DirectX::XMFLOAT2 GetOffset() const { return m_Offset; }
		void SetOffset(const DirectX::XMFLOAT2& offset) { m_Offset = offset; }

		bool SaveToFile(const std::string& filepath) override;
		bool LoadFromFile(const std::string& filepath) override;

	private:
	private:
		// shader
		// rendering mode

		// Main Maps
		std::unordered_map<eMaterialMapType, Texture2D*> m_MainMaps;
		DirectX::XMFLOAT4 m_AlbedoColor = { 1.0f, 1.0f ,1.0f, 1.0f };
		DirectX::XMFLOAT2 m_Tiling = { 1.0f, 1.0f };
		DirectX::XMFLOAT2 m_Offset = { 0.0f, 0.0f };

		unsigned int m_Flags = 0;

		// Second Maps
	};
}