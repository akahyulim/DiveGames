#include "EditorScene.h"

EditorScene::EditorScene(const std::string name)
	: Dive::Scene(name)
{
}

void EditorScene::UpdateEditor(float elapsedTime, EditorCamera* pCamera)
{
	// begin: camera ����
	Dive::BeginScene();

	// temp: �׷��� GameObject dirty check�� �� ������ �ϰ� �ִ�.
	Dive::Scene::Update(elapsedTime);

	// �� ���� �׷��� ������ ���� �� draw�Լ��� �����Ѵ�.

	// end
	Dive::EndScene();
}
