#include "divepch.h"
#include "Material.h"
#include "Core/Context.h"
#include "IO/Log.h"

namespace Dive
{
    Material::Material(Context* pContext)
        : Resource(pContext),
        m_pScene(nullptr),
        m_pTechnique(nullptr)
    {
    }

    Material::~Material()
    {
        DV_LOG_ENGINE_TRACE("Mateiral ¼Ò¸êÀÚ È£Ãâ");
    }

    void Material::SetTechnique(Technique* pTech)
    {
        if (m_pTechnique != pTech)
            m_pTechnique = pTech;
    }

    bool Material::Load(FileStream* pDeserializer)
    {
        return false;
    }

    void Material::RegisterObject(Context* pContext)
    {
        pContext->RegisterFactory<Material>();
    }
}