#pragma once
#include "GraphicsDevice.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Event.h"

// override this(fucntion) to perform custom gameplay or rendering logic
namespace dive
{
	class RenderPath
	{
	public:
		RenderPath()
		{
			EVENT_SUBSCRIBE(eEventType::SceneActivate, EVENT_HANDLE(OnSetActiveScene));
		}
		virtual ~RenderPath() {}

		// LoadingRenderPath에서 호출된다.
		virtual void Load() {}

		// renderpath가 swap될 때 호출?
		virtual void Start() {}
		virtual void Stop() {}

		virtual void PreUpdate() {}
		virtual void FixedUpdate() {}
		virtual void Update(float deltaTime) {}
		virtual void PostUpdate() {}
		virtual void Render() const {}

		// 아직 뭔지 잘 모르겠다.
		virtual void Compose() const {}

		// get & set mask

		void OnSetActiveScene()
		{
			m_pScene = SceneManager::GetInstance().GetActiveScene();
		}

	protected:
		Scene* m_pScene;

	private:
		// mask

	};
}