#include "EditorScene.h"

EditorScene::EditorScene(const std::string name)
	: Dive::Scene(name)
{
}

// �� ������ �׸��� ������ Hazel�� ECS�� �׷� �� �ִ�.
// ���� ��¼�� GameObject�� Update�� ���� Renderer Component���� Draw �Ǿ�� �� �� �ִ�.
void EditorScene::UpdateEditor(float elapsedTime, EditorCamera* pCamera)
{
	// begin / end scene��
	// ����ȭ�� Renderer�� ���� ������ ȣ������.
	// �ֳ��ϸ� Scene ��ü�� Engine ��ü�̱� �����̴�.

	// begin: camera ����
	Dive::Renderer::BeginScene();

	// temp: �׷��� GameObject dirty check�� �� ������ �ϰ� �ִ�.
	Dive::Scene::Update(elapsedTime);

	// �� ���� �׷��� ������ ���� �� draw�Լ��� �����Ѵ�.

	// end
	Dive::Renderer::EndScene();
}
