#include "EditorScene.h"

EditorScene::EditorScene(const std::string name)
	: Dive::Scene(name)
{
}

void EditorScene::UpdateEditor(float elapsedTime, EditorCamera* pCamera)
{
	// temp
	Dive::Scene::Update(elapsedTime);
}
