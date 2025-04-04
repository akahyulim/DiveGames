#pragma once
#include "NativeScript.h"

namespace Dive
{
	class Player : public NativeScript
	{
		DV_CLASS(Player, NativeScript)

	public:
		Player(GameObject* gameObject);
		~Player() override;

		void Create() override;
		void Update() override;
		void Destroy() override;

		void OnKeyPress(int keyCode) override;

	private:
	private:
	};
}
