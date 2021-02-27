#pragma once

struct aiScene;
struct aiNode;
struct aiMesh;

// ���������� �ͽ���Ʈ���� �����ؾ� �Ѵ�.
// ����Ǿ��� �������� GameObject�� �����Ѵ�. ������ ����̴�.
// ������ �� ���� �������� ����ȭ�ȴ�.

// ����Ƽ�� ���� ���ڸ� �⺻ ������Ʈ�� �� ������Ʈ�� �ٸ���.
// �⺻ ������Ʈ�� �������� �ƴ϶�� ���̴�.
// ��, ����ȭ�� ���� �ƴ϶� �ڵ带 ���� �����ȴٰ� ������ �� �ִ�.

// �ϴ� �������� transform�� ������ ������ �� �ִµ� �ϴ�.
// �׷��ٸ� ����ȭ�� ��� �Ǵ°ɱ�? �ϳ��� �������� �κ��� �ְ� Transform�� ���� ����ȭ�Ǵ� �ɱ�...?
// �װ� �ƴ϶�� �������� �ε��ϴ� �Լ����� ��ġ�� �о �����ϴ� ���ϱ�?

class ModelImporter
{
public:
	ModelImporter(Dive::Scene* scene);
	~ModelImporter();

	bool LoadFromFile(const std::string& filepath);

private:
	void parseNode(const aiNode* node, Dive::GameObject* object, Dive::GameObject* parentObject = nullptr);
	void parseMesh(const aiNode* node, Dive::GameObject* object);
	void loadMesh(aiMesh* mesh, Dive::GameObject* object);

	// test
	void printModelData(const aiNode* node);

private:
	Dive::Scene* m_scene;
	aiScene* m_aiScene;
};