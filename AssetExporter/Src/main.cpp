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

	Animator animator(&animation);

	// loop
	{
		// delta time

		// animator.UpdateAnimation(deltaTime);

		// ��� ������ �ִϸ��̼� ���� ��ȯ ��� ȹ��
		// animator.GetFinalBoneMatrices();

		// model.Draw();
	}

	return 0;
}