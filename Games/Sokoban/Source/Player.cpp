#include "Player.h"
#include "Graphics/TextureManager.h"
#include <algorithm>

namespace Sokoban
{
	void Player::SetFrameHandle(ePlayerDir dir, int frame, Dive::TextureHandle handle)
	{
		const int d = std::clamp(static_cast<int>(dir), 0, DirCount - 1);
		const int f = std::clamp(frame, 0, FrameCount - 1);
		m_frames[static_cast<size_t>(d * FrameCount + f)] = handle;
	}

	void Player::SetDirection(ePlayerDir dir)
	{
		m_dir = dir;
	}

	void Player::SetMoveDuration(float durationSeconds)
	{
		m_duration = std::max(0.0f, durationSeconds);
	}

	/*
	void Player::StartMove(const DirectX::XMINT2& fromGrid, const DirectX::XMINT2& toGrid, ePlayerDir dir)
	{
		m_dir = dir;
		m_idleGridPos = toGrid;

		if (m_duration <= 0.0f)
		{
			m_isMoving = false;
			m_elapsed = 0.0f;
			m_animFrame = 0;
			m_startGridPos = toGrid;
			m_targetGridPos = toGrid;
			return;
		}

		m_startGridPos = fromGrid;
		m_targetGridPos = toGrid;
		m_elapsed = 0.0f;
		m_animFrame = 0;
		m_isMoving = true;
	}
	*/
	void Player::StartMove(const DirectX::XMINT2& fromGrid, const DirectX::XMINT2& toGrid, ePlayerDir dir)
	{
		m_dir = dir;
		m_idleGridPos = toGrid;

		if (m_duration <= 0.0f)
		{
			m_isMoving = false;
			m_elapsed = 0.0f;
			m_animFrame = 0;
			m_startGridPos = toGrid;
			m_targetGridPos = toGrid;
			return;
		}

		m_startGridPos = fromGrid;
		m_targetGridPos = toGrid;
		m_elapsed = 0.0f;

		// 한 칸 이동당 한 프레임(1 또는 2)만 사용하도록 토글
		m_stepToggle = !m_stepToggle;
		m_animFrame = m_stepToggle ? 1 : 2;

		m_isMoving = true;
	}

	void Player::SnapToGrid(const DirectX::XMINT2& gridPos)
	{
		m_isMoving = false;
		m_elapsed = 0.0f;
		m_animFrame = 0;
		m_idleGridPos = gridPos;
		m_startGridPos = gridPos;
		m_targetGridPos = gridPos;
	}

	/*
	void Player::Update(float dt)
	{
		if (!m_isMoving)
		{
			m_animFrame = 0;
			return;
		}

		const float safeDt = std::max(0.0f, dt);
		m_elapsed += safeDt;

		const float t = (m_duration > 0.0f)
			? std::clamp(m_elapsed / m_duration, 0.0f, 1.0f)
			: 1.0f;

		// 이동 구간 [0.0, 1.0]을 3등분해 0 -> 1 -> 2 프레임으로 순환
		m_animFrame = std::min(FrameCount - 1, static_cast<int>(t * static_cast<float>(FrameCount)));

		if (t >= 1.0f)
		{
			m_isMoving = false;
			m_elapsed = 0.0f;
			m_animFrame = 0;
			m_idleGridPos = m_targetGridPos;
		}
	}
	*/

	void Player::Update(float dt)
	{
		if (!m_isMoving)
		{
			m_animFrame = 0;
			return;
		}

		const float safeDt = std::max(0.0f, dt);
		m_elapsed += safeDt;

		if (m_elapsed >= m_duration)
		{
			m_isMoving = false;
			m_elapsed = 0.0f;
			m_animFrame = 0;
			m_idleGridPos = m_targetGridPos;
		}
	}

	DirectX::XMFLOAT2 Player::GetRenderGridPos() const
	{
		if (!m_isMoving || m_duration <= 0.0f)
		{
			return DirectX::XMFLOAT2{
				static_cast<float>(m_idleGridPos.x),
				static_cast<float>(m_idleGridPos.y)
			};
		}

		const float t = std::clamp(m_elapsed / m_duration, 0.0f, 1.0f);
		const float x = static_cast<float>(m_startGridPos.x) +
			(static_cast<float>(m_targetGridPos.x) - static_cast<float>(m_startGridPos.x)) * t;
		const float y = static_cast<float>(m_startGridPos.y) +
			(static_cast<float>(m_targetGridPos.y) - static_cast<float>(m_startGridPos.y)) * t;

		return DirectX::XMFLOAT2{ x, y };
	}

	Dive::TextureHandle Player::GetCurrentHandle() const
	{
		const int d = std::clamp(static_cast<int>(m_dir), 0, DirCount - 1);
		const int f = std::clamp(m_animFrame, 0, FrameCount - 1);
		return m_frames[static_cast<size_t>(d * FrameCount + f)];
	}
}