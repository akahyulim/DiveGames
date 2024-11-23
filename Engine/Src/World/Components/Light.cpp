#include "DivePch.h"
#include "Light.h"
#include "Core/CoreDefs.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Texture2DArray.h"
#include "Graphics/Cubemap.h"
#include "Graphics/ConstantBuffer.h"
#include "World/GameObject.h"
#include "Camera.h"
#include "Graphics/Graphics.h"

using namespace DirectX;

namespace Dive
{
	const static float s_NearPlane = 5.0f;

	Light::Light(GameObject* pGameObject, eLightType type)
		: Component(pGameObject)
		, m_Type(type)
		, m_Color{1.0f, 1.0f, 1.0f}
		, m_pShadowMap(nullptr)
		, m_ShadowMapSize(0.0f)
		, m_bShadowEnabled(false)
		, m_pCBufferVS(nullptr)
		, m_pCBufferPS(nullptr)
	{
	}

	Light::~Light()
	{
		DV_DELETE(m_pShadowMap);

		DV_DELETE(m_pCBufferPS);
		DV_DELETE(m_pCBufferVS);
	}

	XMMATRIX Light::GetShadowViewProj()
	{
		return XMMatrixIdentity();
	}

	CascadedMatrixSet::CascadedMatrixSet()
		: m_CascadeTotalRange(50.0f)
		, m_pCamera(nullptr)
	{
	}

	void CascadedMatrixSet::Initialize(uint32_t size, GameObject* pCamera)
	{
		m_MapSize = size;
		m_pCamera = pCamera;
		
		m_ArrCascadeRagnes[0] = m_pCamera->GetComponent<Camera>()->GetNearClipPlane();
		m_ArrCascadeRagnes[1] = 10.0f;
		m_ArrCascadeRagnes[2] = 25.0f;
		m_ArrCascadeRagnes[3] = m_CascadeTotalRange;

		for (uint32_t i = 0; i < 3; i++)
		{
			m_ArrCascadeBoundCenter[i] = { 0.0f, 0.0f, 0.0f };
			m_ArrCascadeBoundRadius[i] = 0.0f;
		}
	}

	void CascadedMatrixSet::Update(const DirectX::XMVECTOR& dir)
	{
		auto pos = XMVectorAdd(m_pCamera->GetPositionVector(), XMVectorScale(m_pCamera->GetForwardVector(), m_CascadeTotalRange * 0.5f));
		auto focus = XMVectorAdd(pos, XMVectorScale(dir, m_pCamera->GetComponent<Camera>()->GetFarClipPlane()));
		auto view = XMMatrixLookAtLH(pos, focus, m_pCamera->GetUpwardVector());

		XMVECTOR boundCenter;
		float radius;
		extractFrustumBoundSphere(m_ArrCascadeRagnes[0], m_ArrCascadeRagnes[3], boundCenter, radius);
		XMStoreFloat3(&m_ShadowBoundCenter, boundCenter);
		m_ShadowBoundRadius = std::max(m_ShadowBoundRadius, radius);

		auto proj = XMMatrixOrthographicLH(-m_ShadowBoundRadius, m_ShadowBoundRadius, -m_ShadowBoundRadius, m_ShadowBoundRadius);

		auto worldToSpace = XMMatrixMultiply(view, proj);
		XMStoreFloat4x4(&m_WorldToShadowSpace, worldToSpace);

		for (uint32_t i = 0; i < 3; i++)
		{
			XMMATRIX cascadeTrans;
			XMMATRIX cascadeScale;

			{
				XMVECTOR newCenter;
				extractFrustumBoundSphere(m_ArrCascadeRagnes[i], m_ArrCascadeRagnes[i + 1], newCenter, radius);
				m_ArrCascadeBoundRadius[i] = std::max(m_ArrCascadeBoundRadius[i], radius);

				XMVECTOR offset;
				if (cascadeNeedsUpdate(view, i, newCenter, offset))
				{
					XMVECTOR offsetOut;
					offsetOut = XMVector3TransformNormal(offset, XMMatrixTranspose(view));

					XMVECTOR boundCenter = XMLoadFloat3(&m_ArrCascadeBoundCenter[i]);
					boundCenter += offsetOut;
					XMStoreFloat3(&m_ArrCascadeBoundCenter[i], boundCenter);
				}

				XMVECTOR centerShadowSpace = XMVector3TransformCoord(boundCenter, worldToSpace);

				m_ToCascadeOffsetX[i] = -XMVectorGetX(centerShadowSpace);
				m_ToCascadeOffsetY[i] = -XMVectorGetY(centerShadowSpace);
				cascadeTrans = XMMatrixTranslation(m_ToCascadeOffsetX[i], m_ToCascadeOffsetY[i], 0.0f);

				m_ToCascadeScale[i] = m_ShadowBoundRadius / m_ArrCascadeBoundRadius[i];
				cascadeScale = XMMatrixScaling(m_ToCascadeScale[i], m_ToCascadeScale[i], 1.0f);
			}

			auto worldToProj = worldToSpace * cascadeTrans * cascadeScale;
			XMStoreFloat4x4(&m_ArrWorldToCascadeProj[i], worldToProj);
		}

		// 상당히 이상하다.
		m_ToCascadeOffsetX[3] = 250.0f;
		m_ToCascadeOffsetY[3] = 250.0f;
		m_ToCascadeScale[3] = 0.1f;
	}

	void CascadedMatrixSet::extractFrustumPoints(float nearClip, float farClip, DirectX::XMVECTOR* pArrFrustumCorners)
	{
		const auto& camPos = m_pCamera->GetPositionVector();
		const auto& camRight = m_pCamera->GetRightwardVector();
		const auto& camUp = m_pCamera->GetUpwardVector();
		const auto& camForward = m_pCamera->GetForwardVector();

		auto pCameraCom = m_pCamera->GetComponent<Camera>();

		const float tanFovX = tanf(pCameraCom->GetAspectRatio() * pCameraCom->GetFieldOfView());
		const float tanFovY = tanf(pCameraCom->GetAspectRatio());

		pArrFrustumCorners[0] = XMVectorAdd(camPos, XMVectorScale(XMVectorAdd(XMVectorAdd(XMVectorNegate(XMVectorScale(camRight, tanFovX)), XMVectorScale(camUp, tanFovY)), camForward), nearClip));
		pArrFrustumCorners[1] = XMVectorAdd(camPos, XMVectorScale(XMVectorAdd(XMVectorAdd(XMVectorScale(camRight, tanFovX), XMVectorScale(camUp, tanFovY)), camForward), nearClip));
		pArrFrustumCorners[2] = XMVectorAdd(camPos, XMVectorScale(XMVectorAdd(XMVectorAdd(XMVectorScale(camRight, tanFovX), XMVectorNegate(XMVectorScale(camUp, tanFovY))), camForward), nearClip));
		pArrFrustumCorners[3] = XMVectorAdd(camPos, XMVectorScale(XMVectorAdd(XMVectorAdd(XMVectorNegate(XMVectorScale(camRight, tanFovX)), XMVectorNegate(XMVectorScale(camUp, tanFovY))), camForward), nearClip));

		pArrFrustumCorners[4] = XMVectorAdd(camPos, XMVectorScale(XMVectorAdd(XMVectorAdd(XMVectorNegate(XMVectorScale(camRight, tanFovX)), XMVectorScale(camUp, tanFovY)), camForward), farClip));
		pArrFrustumCorners[5] = XMVectorAdd(camPos, XMVectorScale(XMVectorAdd(XMVectorAdd(XMVectorScale(camRight, tanFovX), XMVectorScale(camUp, tanFovY)), camForward), farClip));
		pArrFrustumCorners[6] = XMVectorAdd(camPos, XMVectorScale(XMVectorAdd(XMVectorAdd(XMVectorScale(camRight, tanFovX), XMVectorNegate(XMVectorScale(camUp, tanFovY))), camForward), farClip));
		pArrFrustumCorners[7] = XMVectorAdd(camPos, XMVectorScale(XMVectorAdd(XMVectorAdd(XMVectorNegate(XMVectorScale(camRight, tanFovX)), XMVectorNegate(XMVectorScale(camUp, tanFovY))), camForward), farClip));
	}

	void CascadedMatrixSet::extractFrustumBoundSphere(float nearClip, float farClip, DirectX::XMVECTOR& boundCenter, float& boundRadius)
	{
		auto pCameraCom = m_pCamera->GetComponent<Camera>();

		const float tanFovX = tanf(pCameraCom->GetAspectRatio() * pCameraCom->GetFieldOfView());
		const float tanFovY = tanf(pCameraCom->GetAspectRatio());

		auto cameraPos = m_pCamera->GetPositionVector();
		auto cameraForward = m_pCamera->GetForwardVector();
		boundCenter = XMVectorAdd(cameraPos, XMVectorScale(cameraForward, nearClip + 0.5f * (nearClip + farClip)));

		auto cameraRight = m_pCamera->GetRightwardVector();
		auto cameraUp = m_pCamera->GetUpwardVector();
		auto rightComponent = XMVectorNegate(XMVectorScale(cameraRight, tanFovX));
		auto upComponent = XMVectorScale(cameraUp, tanFovY);
		auto forwardComponent = cameraForward;

		auto boundSpan = XMVectorAdd(cameraPos, XMVectorScale(XMVectorAdd(XMVectorAdd(rightComponent, upComponent), forwardComponent), farClip));

		boundRadius = XMVectorGetX(XMVector3Length(boundSpan));
	}

	bool CascadedMatrixSet::cascadeNeedsUpdate(const DirectX::XMMATRIX& shadowView, int cascadeIndex, const DirectX::XMVECTOR& newCenter, DirectX::XMVECTOR& offset)
	{
		auto oldCenterInCascade = XMVector3TransformCoord(XMLoadFloat3(&m_ArrCascadeBoundCenter[cascadeIndex]), shadowView);
		auto newCenterInCascade = XMVector3TransformCoord(newCenter, shadowView);
		auto centerDiff = newCenterInCascade - oldCenterInCascade;

		auto pixelSize = static_cast<float>(m_MapSize) / (2.0f * m_ArrCascadeBoundRadius[cascadeIndex]);

		auto pixelOffX = XMVectorGetX(centerDiff) * pixelSize;
		auto pixelOffY = XMVectorGetY(centerDiff) * pixelSize;

		bool bNeedUpdate = std::abs(pixelOffX) > 0.5f || std::abs(pixelOffY) > 0.5f;
		if (bNeedUpdate)
		{
			offset = { std::floorf(0.5f + pixelOffX) / pixelSize,
				std::floorf(0.5f + pixelOffY) / pixelSize,
				XMVectorGetZ(centerDiff) };
		}

		return bNeedUpdate;
	}

#pragma pack(push, 1)
	struct CB_DIRECTIONAL_VS
	{
		// matViewProj
	};

	struct CB_DIRECTIONAL_GS
	{
		XMFLOAT4X4 cascadeViewProj[3];
	};

	struct CB_DIRECTIONAL_PS
	{
		XMFLOAT3 lightColor;
		float padding;

		XMFLOAT3 dirToLight;
		uint32_t shadowEnabled;

		XMFLOAT4X4 toShadowSpace;
		XMFLOAT4 toCascadeOffsetX;
		XMFLOAT4 toCascadeOffsetY;
		XMFLOAT4 toCascadeScale;
	};
#pragma pack(pop)

	DirectionalLight::DirectionalLight()
		: Light(nullptr, eLightType::Directional)
		, m_pCBufferGS(nullptr)
	{
		m_ShadowMapSize = 1024.0f;
		m_pShadowMap = static_cast<Texture*>(new Texture2DArray(1024, 1024, 32, 3));
	}

	DirectionalLight::DirectionalLight(GameObject* pGameObject)
		: Light(pGameObject, eLightType::Directional)
		, m_pCBufferGS(nullptr)
	{
		m_ShadowMapSize = 1024.0f;
		m_pShadowMap = static_cast<Texture*>(new Texture2DArray(1024, 1024, 32, 3));
	}
	
	// 생성자에 넣을 수 없어 별도의 메서드로 분리했다.
	void DirectionalLight::InitializeCascadedMatrixSet(GameObject* pCamera)
	{
		m_CascadedMatrixSet.Initialize(1024, pCamera);
	}

	void DirectionalLight::Update()
	{
		m_CascadedMatrixSet.Update(m_pGameObject->GetForwardVector());
	
		return;

		// gs constant buffer
		{
			if (!m_pCBufferGS)
				m_pCBufferGS = ConstantBuffer::Create("CB_DIRECTIONAL_GS", sizeof(CB_DIRECTIONAL_GS));

			auto pMappedData = reinterpret_cast<CB_DIRECTIONAL_GS*>(m_pCBufferGS->Map());
			for (uint32_t i = 0; i != 3; i++)
			{
				XMStoreFloat4x4(&pMappedData->cascadeViewProj[i], 
					XMMatrixTranspose(m_CascadedMatrixSet.GetWorldToCascadeProj(i)));
			}
			m_pCBufferGS->Unmap();
		}
		
		// ps constant buffer
		{
			if (!m_pCBufferPS)
				m_pCBufferPS = ConstantBuffer::Create("CB_DIRECTIONAL_PS", sizeof(CB_DIRECTIONAL_PS));

			auto pMappedData = reinterpret_cast<CB_DIRECTIONAL_PS*>(m_pCBufferPS->Map());
			
			pMappedData->lightColor = m_Color;
			XMStoreFloat3(&pMappedData->dirToLight, XMVectorNegate(m_pGameObject->GetForwardVector()));
			pMappedData->shadowEnabled = m_bShadowEnabled ? 1 : 0;
			XMStoreFloat4x4(&pMappedData->toShadowSpace, XMMatrixTranspose(m_CascadedMatrixSet.GetWorldToShadowSpace()));
			XMStoreFloat4(&pMappedData->toCascadeOffsetX, m_CascadedMatrixSet.GetToCascadeOffsetX());
			XMStoreFloat4(&pMappedData->toCascadeOffsetY, m_CascadedMatrixSet.GetToCascadeOffsetY());
			XMStoreFloat4(&pMappedData->toCascadeScale, m_CascadedMatrixSet.GetToCascadeScale());
			
			m_pCBufferPS->Unmap();
		}
	}

	ConstantBuffer* DirectionalLight::GetConstantBufferVS()
	{
		return nullptr;
	}

	ConstantBuffer* DirectionalLight::GetConstantBufferDS()
	{
		return nullptr;
	}

	ConstantBuffer* DirectionalLight::GetConstantBufferGS()
	{
		if (!m_pCBufferGS)
			m_pCBufferGS = ConstantBuffer::Create("CB_DIRECTIONAL_GS", sizeof(CB_DIRECTIONAL_GS));

		auto pMappedData = reinterpret_cast<CB_DIRECTIONAL_GS*>(m_pCBufferGS->Map());
		for (uint32_t i = 0; i != 3; i++)
		{
			XMStoreFloat4x4(&pMappedData->cascadeViewProj[i],
				XMMatrixTranspose(m_CascadedMatrixSet.GetWorldToCascadeProj(i)));
		}
		m_pCBufferGS->Unmap();

		return m_pCBufferGS;
	}

	ConstantBuffer* DirectionalLight::GetConstantBufferPS()
	{
		if (!m_pCBufferPS)
			m_pCBufferPS = ConstantBuffer::Create("CB_DIRECTIONAL_PS", sizeof(CB_DIRECTIONAL_PS));

		auto pMappedData = reinterpret_cast<CB_DIRECTIONAL_PS*>(m_pCBufferPS->Map());
		{
			pMappedData->lightColor = m_Color;
			XMStoreFloat3(&pMappedData->dirToLight, XMVectorNegate(m_pGameObject->GetForwardVector()));
			pMappedData->shadowEnabled = m_bShadowEnabled ? 1 : 0;
			XMStoreFloat4x4(&pMappedData->toShadowSpace, XMMatrixTranspose(m_CascadedMatrixSet.GetWorldToShadowSpace()));
			XMStoreFloat4(&pMappedData->toCascadeOffsetX, m_CascadedMatrixSet.GetToCascadeOffsetX());
			XMStoreFloat4(&pMappedData->toCascadeOffsetY, m_CascadedMatrixSet.GetToCascadeOffsetY());
			XMStoreFloat4(&pMappedData->toCascadeScale, m_CascadedMatrixSet.GetToCascadeScale());
		}
		m_pCBufferPS->Unmap();

		return m_pCBufferPS;
	}

#pragma pack(push, 1)
	struct CB_POINT_DS
	{
		XMFLOAT4X4 world;
	};

	struct CB_POINT_GS
	{
		XMFLOAT4X4 cubeViewProj[6];
	};

	struct CB_POINT_PS
	{
		XMFLOAT3 lightColor;
		float lightRangeRcp;

		XMFLOAT3 lightPos;
		uint32_t shadowEnabled;

		XMFLOAT2 lightPerspectiveValues;
		float padding[2];
	};
#pragma pack(pop)

	PointLight::PointLight()
		: Light(nullptr, eLightType::Point)
		, m_pCBufferDS(nullptr)
		, m_pCBufferGS(nullptr)
		, m_Range(0.0f)
	{
		m_ShadowMapSize = 1024.0f;
		m_pShadowMap = static_cast<Texture*>(new Cubemap(1024, 32));
	}

	PointLight::PointLight(GameObject* pGameObject)
		: Light(pGameObject, eLightType::Point)
		, m_pCBufferDS(nullptr)
		, m_pCBufferGS(nullptr)
		, m_Range(0.0f)
	{
		m_ShadowMapSize = 1024.0f;
		m_pShadowMap = static_cast<Texture*>(new Cubemap(1024, 32));
	}

	PointLight::~PointLight()
	{
		DV_DELETE(m_pCBufferGS);
		DV_DELETE(m_pCBufferDS);
	}

	void PointLight::Update()
	{
		return;
	}
	
	ConstantBuffer* PointLight::GetConstantBufferDS()
	{
		if (!m_pCBufferDS)
			m_pCBufferDS = ConstantBuffer::Create("CB_POINT_DS", sizeof(CB_POINT_DS));

		auto scale = XMMatrixScaling(m_Range, m_Range, m_Range);
		auto trans = XMMatrixTranslationFromVector(m_pGameObject->GetPositionVector());

		auto pMappedData = reinterpret_cast<CB_POINT_DS*>(m_pCBufferDS->Map());
		{
			XMStoreFloat4x4(&pMappedData->world, XMMatrixTranspose(scale * trans));
		}
		m_pCBufferDS->Unmap();

		return m_pCBufferDS;
	}

	// 이 값은 월드변환된 오브젝트를 큐브맵을 이루는 6개의 각 면으로 변환하기 위한 뷰, 프로젝션 행렬이다.
	ConstantBuffer* PointLight::GetConstantBufferGS()
	{
		if (!m_pCBufferGS)
			m_pCBufferGS = ConstantBuffer::Create("CB_POINT_GS", sizeof(CB_POINT_GS));

		auto center = XMMatrixTranslationFromVector(XMVectorNegate(m_pGameObject->GetPositionVector()));
		auto proj = XMMatrixPerspectiveFovLH(XM_PIDIV2, 1.0f, s_NearPlane, m_Range);

		auto pMappedData = reinterpret_cast<CB_POINT_GS*>(m_pCBufferGS->Map());
		{
			// Cube +X
			XMMATRIX view =  center * XMMatrixRotationY(-XM_PIDIV2);
			XMStoreFloat4x4(&pMappedData->cubeViewProj[0], XMMatrixTranspose(view * proj));

			// Cube -X
			view = center * XMMatrixRotationY(XM_PIDIV2);
			XMStoreFloat4x4(&pMappedData->cubeViewProj[1], XMMatrixTranspose(view * proj));

			// Cube +Y
			view = center * XMMatrixRotationX(XM_PIDIV2);
			XMStoreFloat4x4(&pMappedData->cubeViewProj[2], XMMatrixTranspose(view * proj));

			// Cube -Y
			view = center * XMMatrixRotationX(-XM_PIDIV2);
			XMStoreFloat4x4(&pMappedData->cubeViewProj[3], XMMatrixTranspose(view * proj));

			// Cube +Z
			view = center;
			XMStoreFloat4x4(&pMappedData->cubeViewProj[4], XMMatrixTranspose(view * proj));

			// Cube -Z
			view = center * XMMatrixRotationY(XM_PI);
			XMStoreFloat4x4(&pMappedData->cubeViewProj[5], XMMatrixTranspose(view * proj));
		}
		m_pCBufferGS->Unmap();

		return m_pCBufferGS;;
	}

	ConstantBuffer* PointLight::GetConstantBufferPS()
	{
		if (!m_pCBufferPS)
			m_pCBufferPS = ConstantBuffer::Create("CB_POINT_PS", sizeof(CB_POINT_PS));

		auto pMappedData = reinterpret_cast<CB_POINT_PS*>(m_pCBufferPS->Map());
		{
			pMappedData->lightColor = m_Color;
			pMappedData->lightRangeRcp = 1 / m_Range;
			pMappedData->lightPos = m_pGameObject->GetPosition();
			pMappedData->shadowEnabled = m_bShadowEnabled ? 1 : 0;

			if (m_bShadowEnabled)
			{
				auto proj = XMMatrixPerspectiveFovLH(XM_PIDIV2, 1.0f, s_NearPlane, m_Range);
				XMFLOAT4X4 projFloat4x4;
				XMStoreFloat4x4(&projFloat4x4, proj);
				pMappedData->lightPerspectiveValues = { projFloat4x4._33, projFloat4x4._43 };
			}
		}
		m_pCBufferPS->Unmap();

		return m_pCBufferPS;
	}

#pragma pack(push, 1)
	struct CB_SPOTLIGHT_VS
	{
		XMFLOAT4X4 viewProj;
	};

	struct CB_SPOTLIGHT_DS
	{
		XMFLOAT4X4 world;

		float sinOuter;
		float cosOuter;
		float padding[2];
	};

	struct CB_SPOTLIGHT_PS
	{
		XMFLOAT3 lightColor;
		float lightRangeRcp;

		XMFLOAT3 lightPos;
		float cosOuterCone;

		XMFLOAT3 dirToLight;
		float cosConeAttRange;

		XMFLOAT4X4 viewProj;

		uint32_t shadowEnabled;
		float padding[3];
	};
#pragma pack(pop)

	SpotLight::SpotLight()
		: Light(nullptr, eLightType::Spot)
		, m_pCBufferDS(nullptr)
		, m_Range(0.0f)
		, m_InnerAngle(0.0f)
		, m_OuterAngle(0.0f)
	{
		// 임시 위치
		// 기본적으로 nullptr이 맞고
		// 그림자를 적용하고자 할 때 생성하는 것이 맞는 듯 하다.
		// 다만 현재 크기가 고정되어 있다.
		m_ShadowMapSize = 1024.0f;
		m_pShadowMap = static_cast<Texture*>(new Texture2D(1024, 1024, 32));
	}

	SpotLight::SpotLight(GameObject* pGameObject)
		: Light(pGameObject, eLightType::Spot)
		, m_pCBufferDS(nullptr)
		, m_Range(0.0f)
		, m_InnerAngle(0.0f)
		, m_OuterAngle(0.0f)
	{
		m_ShadowMapSize = 1024.0f;
		m_pShadowMap = static_cast<Texture*>(new Texture2D(1024, 1024, 32));
	}

	SpotLight::~SpotLight()
	{
		DV_DELETE(m_pCBufferDS);
	}

	void SpotLight::Update()
	{
		auto viewProj = GetShadowViewProj();

		// vs constant buffer
		{
			if (!m_pCBufferVS)
				m_pCBufferVS = ConstantBuffer::Create("CB_SPOTLIGHT_VS", sizeof(CB_SPOTLIGHT_VS));

			auto pMappedData = reinterpret_cast<CB_SPOTLIGHT_VS*>(m_pCBufferVS->Map());
			XMStoreFloat4x4(&pMappedData->viewProj, XMMatrixTranspose(viewProj));
			m_pCBufferVS->Unmap();
		}

		float outerRad = XMConvertToRadians(m_OuterAngle);
		float innerRad = XMConvertToRadians(m_InnerAngle);

		float sinOuter = sinf(outerRad);
		float cosOuter = cosf(outerRad);
		float cosInner = cosf(innerRad);

		// ds constant buffer
		{
			if (!m_pCBufferDS)
				m_pCBufferDS = ConstantBuffer::Create("CB_SPOTLIGHT_DS", sizeof(CB_SPOTLIGHT_DS));

			m_pGameObject->SetScale(m_Range, m_Range, m_Range);
			
			auto pMappedData = reinterpret_cast<CB_SPOTLIGHT_DS*>(m_pCBufferDS->Map());
			XMStoreFloat4x4(& pMappedData->world, XMMatrixTranspose(m_pGameObject->GetMatrix()));
			pMappedData->sinOuter = sinOuter;
			pMappedData->cosOuter = cosOuter;
			m_pCBufferDS->Unmap();
		}

		// ps constant buffer
		{
			if (!m_pCBufferPS)
				m_pCBufferPS = ConstantBuffer::Create("CB_SPOTLIGHT_PS", sizeof(CB_SPOTLIGHT_PS));

			auto pMappedData = reinterpret_cast<CB_SPOTLIGHT_PS*>(m_pCBufferPS->Map());
			pMappedData->lightColor = m_Color;
			pMappedData->lightRangeRcp = 1.0f / m_Range;
			pMappedData->lightPos = m_pGameObject->GetPosition();
			pMappedData->cosOuterCone = cosOuter;
			XMStoreFloat3(&pMappedData->dirToLight, XMVectorNegate(m_pGameObject->GetForwardVector()));
			pMappedData->cosConeAttRange = cosInner - cosOuter;
			XMStoreFloat4x4(&pMappedData->viewProj, XMMatrixTranspose(viewProj));
			pMappedData->shadowEnabled = m_bShadowEnabled ? 1 : 0;
			m_pCBufferPS->Unmap();
		}
	}

	XMMATRIX SpotLight::GetShadowViewProj()
	{
		const auto& pos = m_pGameObject->GetPositionVector();
		const auto& forward = m_pGameObject->GetForwardVector();
		const auto& focus = XMVectorAdd(pos, XMVectorScale(forward, m_Range));

		const auto& up = m_pGameObject->GetUpwardVector();
		
		auto view = XMMatrixLookAtLH(pos, focus, up);

		auto proj = XMMatrixPerspectiveFovLH(
			2.0f * XMConvertToRadians(m_OuterAngle),
			1.0f,			// 가로, 세로 비율이라 1인듯 하다.
			s_NearPlane,			// 책에서도 상수를 멤버 변수로 관리했다. ShadowGen Rasterizer State와 관련이 있나? => 아닌듯?
			m_Range
		);	

		return XMMatrixMultiply(view, proj);
	}
}