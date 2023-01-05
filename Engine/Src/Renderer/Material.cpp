#include "divepch.h"
#include "Material.h"
#include "Renderer.h"
#include "Technique.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Core/Context.h"
#include "Resource/ResourceCache.h"
#include "IO/Log.h"

namespace Dive
{
    Material::Material(Context* pContext)
        : Resource(pContext),
        m_pScene(nullptr),
        m_pTechnique(nullptr)
    {
        ResetToDefault();
    }

    Material::~Material()
    {
        DV_LOG_ENGINE_TRACE("Mateiral �Ҹ� �Ϸ� - {:s}", GetName().empty() ? "No name" : GetName());
    }

    bool Material::Load(FileStream* pDeserializer)
    {
        // ������ ����
        // 1. technique name = technique file name
        // => �������� quality�� �߰��� 2�� �̻����� ������ �� �ִµ�?
        // 2. texture unit = diffuse, name = texture file name
        // => normal�� �߰��� ���� �� �ִ�.
        // 3. parameter name = MatSpecColor, value = 0.1 0.1 0.1 16 => �⺻ ���� ����?
        // => �ؽ��İ� �ִٸ� SpecColor��, ���ٸ� DiffColor�� �����µ�?
        // 4. shader psdefines = PACKEDNORMAL => ���⼭�� defines�� ����ϳ�...

        // �켱 ResetToDefault()�� �����Ѵ�.
        // �̴� ���Ͽ��� ������� ���� ����� �ʱ�ȭ�ϱ� ���� �� �ϴ�.
        // ���� ���� ������ ���� ���ڸ� cullmode, fillmode ���� �׷���.

        // shader
        // vertex, pixel defines�� ��������� �����Ѵ�.

        // technique
        // technique�� ��� �о�� ��
        // qaulity�� loddistance�� �а� SortTechniques(), ApplyShaderDefines()�� �����Ѵ�.

        // textures
        // Ÿ��(unit)�� �̸����� �о�� ��
        // ĳ�÷κ��� ȹ���� ���ҽ��� SetTexture()�� Ÿ�԰� �Բ� �����Ѵ�.
        // => ������ �� �������δ� texture�� sampler state�� �������� ���Ѵ�.
        // => �ᱹ Material���� �����ϴ� ���� �ƴ��� Ȯ���ߴ�.

        // �̿ܿ��� shader parameters, cullmode, fillmode � �о� �´�.

        return false;
    }

    void Material::RegisterObject(Context* pContext)
    {
        pContext->RegisterFactory<Material>();
    }

    void Material::SetTechnique(Technique* pTech)
    {
        if (m_pTechnique != pTech)
            m_pTechnique = pTech;
    }

    Texture* Material::GetTexture(eTextureUnit unit) const
    {
        auto it = m_Textures.find(unit);

        return it != m_Textures.end() ? it->second : nullptr;
    }

    void Material::SetTexture(eTextureUnit unit, Texture* pTexture)
    {
        if (unit < eTextureUnit::Max)
        {
            if (pTexture)
                m_Textures[unit] = pTexture;
            else
                m_Textures.erase(unit); // �̰� �³� �𸣰ڴ�.
        }
    }

    // urho�� load������ �ؽ��ĸ� �ε��ϱ⿡ ���� �����ô�.
    void Material::AddTexture(eTextureUnit unit, const std::string& name)
    {
        if (unit < eTextureUnit::Max)
        {
            ResourceCache* pCache = GetSubsystem<ResourceCache>();
            // ������ Ÿ�Կ� ���缭 �õ��ؾ� �Ѵ�.
            Texture2D* pTexture = pCache->GetResource<Texture2D>(name);
            SetTexture(unit, dynamic_cast<Texture*>(pTexture));
        }
    }

    // ���� ��ũ�� ���� �ܿ� ����ϴ� �κ��� ����.
    void Material::ResetToDefault()
    {
        // �ӽ�
        SetName("DefaultMat");

        // ���⵵ vertex, pixel defeines�� �����ϳ�...

        Renderer* pRenderer = GetSubsystem<Renderer>();
        // ���� �� �ٲ�� �Ѵ�.
        // => ĳ���� GetResource�� ������ �����ؾ� �Ѵٴ� ���̴�.
        if (!pRenderer)
        {
            auto pTech = new Technique(m_pContext);
            pTech->SetName("DefaultTech");
            auto pPass = pTech->CreatePass("base");

            // �۷ι��� ���� ���̴��� �������� �ϴ��� �ϳ��� ���
            // �׸��� urho�� Ȯ���ڵ� ���� �̸��� ����ȴ�.
            // �׷��� Graphics���� �ε��� �� ��θ� ���� ���δ�.
            //pPass->SetVertexShader("CoreData/Shaders/color.hlsl");
            //pPass->SetVertexShaderDefines("VERTEXCOLOR");
            //pPass->SetPixelShader("CoreData/Shaders/color.hlsl");
            //pPass->SetPixelShaderDefines("VERTEXCOLOR");
            pPass->SetVertexShader("CoreData/Shaders/Color.hlsl");
            pPass->SetVertexShaderDefines("VERTEXCOLOR");
            pPass->SetPixelShader("CoreData/Shaders/Color.hlsl");
            pPass->SetPixelShaderDefines("VERTEXCOLOR");

            auto pCache = GetSubsystem<ResourceCache>();
            pCache->AddManualResource(pTech);

            m_pTechnique = pCache->GetResource<Technique>("DefaultTech");
        }
        else
        {
            m_pTechnique = pRenderer->GetDefaultTechnique();
        }


        // shader parameter�� �����Ѵ�.

        // cull mode
        // fill mode
    }
}