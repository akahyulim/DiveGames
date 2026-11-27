#pragma once
#include <array>
#include <DirectXMath.h>

namespace Dive
{
	using TextureHandle = uint64_t;
}

namespace Sokoban
{
	enum class ePlayerDir : int
	{
		Down = 0,
		Up = 1,
		Left = 2,
		Right = 3
	};

	class Player
	{
	public:
		static constexpr int DirCount = 4;
		static constexpr int FrameCount = 3;
		static constexpr int TotalFrames = DirCount * FrameCount; // 12장

	public:
		void SetFrameHandle(ePlayerDir dir, int frame, Dive::TextureHandle handle);
		void SetDirection(ePlayerDir dir);
		void SetMoveDuration(float durationSeconds);

		// 한 칸 이동 보간 및 3프레임 사이클 시작
		void StartMove(const DirectX::XMINT2& fromGrid, const DirectX::XMINT2& toGrid, ePlayerDir dir);
		void SnapToGrid(const DirectX::XMINT2& gridPos);

		// 매 프레임 float dt(초 단위)로 진행
		void Update(float dt);

		bool IsMoving() const { return m_isMoving; }
		ePlayerDir GetDirection() const { return m_dir; }
		int GetAnimFrame() const { return m_animFrame; }

		// Render()에서 그대로 쓸 수 있는 보간된 격자 좌표 및 현재 프레임 텍스처
		DirectX::XMFLOAT2 GetRenderGridPos() const;
		Dive::TextureHandle GetCurrentHandle() const;

	private:
		std::array<Dive::TextureHandle, TotalFrames> m_frames{};

		ePlayerDir m_dir = ePlayerDir::Down;
		int m_animFrame = 0; // 0, 1, 2

		bool m_isMoving = false;
		float m_elapsed = 0.0f;
		float m_duration = 0.14f;

		DirectX::XMINT2 m_idleGridPos{ 0, 0 };
		DirectX::XMINT2 m_startGridPos{ 0, 0 };
		DirectX::XMINT2 m_targetGridPos{ 0, 0 };

		bool m_stepToggle = false;
	};
}