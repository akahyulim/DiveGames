#pragma once

struct aiScene;
struct aiNode;
struct aiMesh;

// 임포터지만 익스포트까지 지원해야 한다.
// 어찌되었건 계층구조 GameObject로 생성한다. 프리팹 대상이다.
// 저장할 때 엔진 포멧으로 파일화된다.

// 유니티를 예로 들자면 기본 오브젝트와 모델 오브젝트는 다르다.
// 기본 오브젝트는 프리팹이 아니라는 말이다.
// 즉, 파일화된 것이 아니라 코드를 통해 생성된다고 유추할 수 있다.

// 일단 프리팹은 transform을 개별로 설정할 수 있는듯 하다.
// 그렇다면 파일화는 어떻게 되는걸까? 하나의 공통적인 부분이 있고 Transform만 따로 파일화되는 걸까...?
// 그게 아니라면 프리팹을 로드하는 함수에서 위치만 읽어서 생성하는 것일까?

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