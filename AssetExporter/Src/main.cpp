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
	// currentAnimation에 위의 animation을 등록
	Animator animator(&animation);

	// loop
	{
		// delta time

		// 시간에 맞춰 모든 bone을 계층구조에 맞춰 누적변환
		// animator.UpdateAnimation(deltaTime);

		// 변환결과를 index에 맞춰 저장한 finalBone 획득
		// animator.GetFinalBoneMatrices();

		// model.Draw();
	}

	return 0;
}