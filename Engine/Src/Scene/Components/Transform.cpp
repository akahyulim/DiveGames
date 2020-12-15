#include "DivePch.h"
#include "Transform.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Scene/GameObject.h"


namespace Dive
{
	Transform::Transform(Context * context, GameObject * owner)
		: Component(context, owner)
	{
		m_type = eComponentType::Transform;

		CORE_TRACE("Create Transform.");
	}

	Transform::~Transform()
	{
		CORE_TRACE("Destroy Transform.");
	}

	void Transform::OnAwake()
	{
		CORE_TRACE("Awake Transform.");
	}
	
	void Transform::OnStart()
	{
		CORE_TRACE("Start Transform.");
	}
	
	void Transform::OnStop()
	{
		CORE_TRACE("Stop Transform.");
	}
	
	void Transform::OnRemove()
	{
		CORE_TRACE("Remove Transform.");
	}
	
	void Transform::OnUpdate()
	{
		CORE_TRACE("Update Transform.");
	}
}