#pragma once
#include "Scene.h"
#include <string>
#include <DirectXMath.h>

namespace Sokoban
{
	class EditorScene : public Scene
	{
	public:
		EditorScene(const GameContext& context);
		~EditorScene() override;

		void Initialize() override;
		void Cleanup() override;

		void Update(float deltaTime) override;

		void Render() override;
		void RenderUI() override;

		void SetGridSize(const DirectX::XMUINT2& size);

	private:

	private:
		std::string m_name = "no name";
		DirectX::XMUINT2 m_gridSize = { 10, 10 };
		float m_tileSize = 1.0f;
		bool m_isGridDirty = true;
	};
}