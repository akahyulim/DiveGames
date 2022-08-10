#pragma once
#include "DvEventSystem.h"

namespace Dive 
{
	// BeginFrame
	class BeginFrameEvent : public DvEvent
	{
		DV_EVENT_CLASS_TYPE(BeginFrame)

	public:
		BeginFrameEvent() = default;
		~BeginFrameEvent() = default;
 
	private:
	};
	
	// Update
	class UpdateEvent : public DvEvent
	{
		DV_EVENT_CLASS_TYPE(Update)

	public:
		UpdateEvent() = default;
		~UpdateEvent() = default;

		float GetDeltaTime() const { return m_DeltaTime; }
		void SetDeltaTime(float delta) { m_DeltaTime = delta; }

	private:
		float m_DeltaTime = 0.0f;
	};

	// PostUpdate
	class PostUpdateEvent : public DvEvent
	{
		DV_EVENT_CLASS_TYPE(PostUpdate)

	public:
		PostUpdateEvent() = default;
		~PostUpdateEvent() = default;

		float GetDeltaTime() const { return m_DeltaTime; }
		void SetDeltaTime(float delta) { m_DeltaTime = delta; }

	private:
		float m_DeltaTime = 0.0f;
	};

	// RenderUpdate
	class RenderUpdateEvent : public DvEvent
	{
		DV_EVENT_CLASS_TYPE(RenderUpdate)

	public:
		RenderUpdateEvent() = default;
		~RenderUpdateEvent() = default;

		float GetDeltaTime() const { return m_DeltaTime; }
		void SetDeltaTime(float delta) { m_DeltaTime = delta; }

	private:
		float m_DeltaTime = 0.0f;
	};
	
	// PostRenderUpdate
	class PostRenderUpdateEvent : public DvEvent
	{
		DV_EVENT_CLASS_TYPE(PostRenderUpdate)

	public:
		PostRenderUpdateEvent() = default;
		~PostRenderUpdateEvent() = default;

		float GetDeltaTime() const { return m_DeltaTime; }
		void SetDeltaTime(float delta) { m_DeltaTime = delta; }

	private:
		float m_DeltaTime = 0.0f;
	};

	// EndFrame
	class EndFrameEvent : public DvEvent
	{
		DV_EVENT_CLASS_TYPE(EndFrame)

	public:
		EndFrameEvent() = default;
		~EndFrameEvent() = default;

	private:
	};
}