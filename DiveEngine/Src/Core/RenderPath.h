#pragma once
#include "Event.h"
#include "../Renderer/Graphics/GraphicsDevice.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Scene.h"

// override this(fucntion) to perform custom gameplay or rendering logic
namespace dive
{
	class RenderPath
	{
	public:
		RenderPath()
			: m_pScene(nullptr)
		{
			EVENT_SUBSCRIBE(eEventType::SceneActivate, EVENT_HANDLE(OnSetActiveScene));
		}
		virtual ~RenderPath() {}

		// LoadingRenderPath���� ȣ��ȴ�.
		virtual void Load() {}

		// renderpath�� swap�� �� ȣ��?
		virtual void Start() {}
		virtual void Stop() {}

		virtual void PreUpdate() {}
		virtual void FixedUpdate() {}
		virtual void Update(float deltaTime) {}
		virtual void PostUpdate() {}
		virtual void Render() const {}
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