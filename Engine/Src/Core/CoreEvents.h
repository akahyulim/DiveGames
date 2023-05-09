#pragma once
#include "EventHandler.h"

namespace Dive
{
	// BeginFrame
	class BeginFrameEvent : public Event
	{
		EVENT_CLASS_TYPE(BeginFrame)

	public:
		BeginFrameEvent(uint32_t frameNum, float timeStep)
			: m_FrameNum(frameNum),
			m_TimeStep(timeStep)
		{}
		~BeginFrameEvent() = default;

		uint32_t GetFrameNum() const { return m_FrameNum; }
		float GetTimeStep() const { return m_TimeStep; }

	private:
		uint32_t m_FrameNum;
		float m_TimeStep;
	};

	// PreUpdate
	class PreUpdateEvent : public Event
	{
		EVENT_CLASS_TYPE(PreUpdate)

	public:
		PreUpdateEvent(float timeStep)
			: m_TimeStep(timeStep)
		{}
		~PreUpdateEvent() = default;

		float GetTimeStep() const { return m_TimeStep; }

	private:
		float m_TimeStep;
	};

	// Update
	class UpdateEvent : public Event
	{
		EVENT_CLASS_TYPE(Update)

	public:
		UpdateEvent(float timeStep)
			: m_TimeStep(timeStep)
		{}
		~UpdateEvent() = default;

		float GetTimeStep() const { return m_TimeStep; }

	private:
		float m_TimeStep;
	};

	// PostUpdate
	class PostUpdateEvent : public Event
	{
		EVENT_CLASS_TYPE(PostUpdate)

	public:
		PostUpdateEvent(float timeStep)
			: m_TimeStep(timeStep)
		{}
		~PostUpdateEvent() = default;

		float GetTimeStep() const { return m_TimeStep; }

	private:
		float m_TimeStep;
	};

	// RenderUpdate
	class RenderUpdateEvent : public Event
	{
		EVENT_CLASS_TYPE(RenderUpdate)

	public:
		RenderUpdateEvent(float timeStep)
			: m_TimeStep(timeStep)
		{}
		~RenderUpdateEvent() = default;

		float GetTimeStep() const { return m_TimeStep; }

	private:
		float m_TimeStep;
	};

	// PostRenderUpdate
	class PostRenderUpdateEvent : public Event
	{
		EVENT_CLASS_TYPE(PostRenderUpdate)

	public:
		PostRenderUpdateEvent(float timeStep)
			: m_TimeStep(timeStep)
		{}
		~PostRenderUpdateEvent() = default;

		float GetTimeStep() const { return m_TimeStep; }

	private:
		float m_TimeStep;
	};

	// EndFrame
	class EndFrameEvent : public Event
	{
		EVENT_CLASS_TYPE(EndFrame)

	public:
		EndFrameEvent() = default;
		~EndFrameEvent() = default;

	private:
	};
}