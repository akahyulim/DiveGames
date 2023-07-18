#include "Model.h"
#include "Animation.h"
#include "Animator.h"

int main(int argc, char** argv)
{
	const char* pFile = "Assets/Models/Stormtrooper/silly_dancing.fbx";
		//"Assets/Models/pilot/Pilot_LP_Animated.fbx";
	
	// Mesh ����
	Model model(pFile);
	// Bone, Hierarcht ����
	Animation animation(pFile, &model);
	// currentAnimation�� ���� animation�� ���
	Animator animator(&animation);

	// loop
	{
		// delta time

		// �ð��� ���� ��� bone�� ���������� ���� ������ȯ
		// animator.UpdateAnimation(deltaTime);

		// ��ȯ����� index�� ���� ������ finalBone ȹ��
		// animator.GetFinalBoneMatrices();

		// model.Draw();
	}

	return 0;
}