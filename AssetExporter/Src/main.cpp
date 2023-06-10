#include "Model.h"
#include "Animation.h"
#include "Animator.h"

int main(int argc, char** argv)
{
	const char* pFile = "Assets/Models/Stormtrooper/silly_dancing.fbx";
		//"Assets/Models/pilot/Pilot_LP_Animated.fbx";
	
	// Mesh 구성
	Model model(pFile);
	// Bone, Hierarcht 구성
	Animation animation(pFile, &model);

	Animator animator(&animation);

	// loop
	{
		// delta time

		// animator.UpdateAnimation(deltaTime);

		// 모든 뼈대의 애니메이션 누적 변환 결과 획득
		// animator.GetFinalBoneMatrices();

		// model.Draw();
	}

	return 0;
}