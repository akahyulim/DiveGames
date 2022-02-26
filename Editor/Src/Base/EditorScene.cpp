#include "EditorScene.h"

EditorScene::EditorScene(const std::string name)
	: Dive::Scene(name)
{
}

// 이 곳에서 그리는 이유는 Hazel이 ECS라 그럴 수 있다.
// 따라서 어쩌면 GameObject의 Update를 통해 Renderer Component에서 Draw 되어야 할 수 있다.
void EditorScene::UpdateEditor(float elapsedTime, EditorCamera* pCamera)
{
	// begin / end scene은
	// 전역화된 Renderer를 직접 가져와 호출하자.
	// 왜냐하면 Scene 자체가 Engine 객체이기 때문이다.

	// begin: camera 전달
	Dive::Renderer::BeginScene();

	// temp: 그런데 GameObject dirty check를 이 곳에서 하고 있다.
	Dive::Scene::Update(elapsedTime);

	// 이 곳은 그려질 대상들을 뽑은 후 draw함수에 전달한다.

	// end
	Dive::Renderer::EndScene();
}
