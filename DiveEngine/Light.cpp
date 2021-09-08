#include "Light.h"
#include "GameObject.h"
#include "FileStream.h"
#include "DiveCore.h"


namespace dive
{
	Light::Light(GameObject* pGameObject)
		: Component(typeid(Light).hash_code(), pGameObject)
	{
		m_Type = eComponentType::Light;

		m_LightType	 = eLightType::Directional;
		m_Color		= DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_Intensity = 1.0f;
		m_Range		= 1.0f;
	}

	Light::~Light()
	{
	}
	
	void Light::Update(float deltaTime)
	{
	}
	
	void Light::Serialize(FileStream* pFileStream)
	{
		DV_ASSERT(pFileStream);

		pFileStream->Write(static_cast<int>(m_LightType));
		pFileStream->Write(m_Color);
		pFileStream->Write(m_Intensity);
		pFileStream->Write(m_Range);
	}
	
	void Light::Deserialize(FileStream* pFileStream)
	{
		DV_ASSERT(pFileStream);

		int type = 0;
		pFileStream->Read(&type);
		m_LightType = static_cast<eLightType>(type);
		pFileStream->Read(&m_Color);
		pFileStream->Read(&m_Intensity);
		pFileStream->Read(&m_Range);
	}
}