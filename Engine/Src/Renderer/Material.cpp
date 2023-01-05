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
        DV_LOG_ENGINE_TRACE("Mateiral 소멸 완료 - {:s}", GetName().empty() ? "No name" : GetName());
    }

    bool Material::Load(FileStream* pDeserializer)
    {
        // 파일의 구성
        // 1. technique name = technique file name
        // => 마지막에 quality를 추가해 2개 이상으로 구성할 수 있는듯?
        // 2. texture unit = diffuse, name = texture file name
        // => normal도 추가로 가질 수 있다.
        // 3. parameter name = MatSpecColor, value = 0.1 0.1 0.1 16 => 기본 색상 정보?
        // => 텍스쳐가 있다면 SpecColor만, 없다면 DiffColor도 가지는듯?
        // 4. shader psdefines = PACKEDNORMAL => 여기서도 defines를 기술하네...

        // 우선 ResetToDefault()를 수행한다.
        // 이는 파일에는 기술하지 않은 멤버를 초기화하기 위한 듯 하다.
        // 위의 파일 구성을 예로 들자면 cullmode, fillmode 등이 그렇다.

        // shader
        // vertex, pixel defines를 멤버변수에 저장한다.

        // technique
        // technique를 모두 읽어온 후
        // qaulity와 loddistance를 읽고 SortTechniques(), ApplyShaderDefines()를 수행한다.

        // textures
        // 타입(unit)과 이름들을 읽어온 후
        // 캐시로부터 획득한 리소스를 SetTexture()에 타입과 함께 전달한다.
        // => 하지만 이 과정으로는 texture에 sampler state를 생성하지 못한다.
        // => 결국 Material에서 생성하는 것이 아님을 확인했다.

        // 이외에도 shader parameters, cullmode, fillmode 등도 읽어 온다.

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
                m_Textures.erase(unit); // 이게 맞나 모르겠다.
        }
    }

    // urho는 load에서만 텍스쳐를 로드하기에 직접 만들어봤다.
    void Material::AddTexture(eTextureUnit unit, const std::string& name)
    {
        if (unit < eTextureUnit::Max)
        {
            ResourceCache* pCache = GetSubsystem<ResourceCache>();
            // 원래는 타입에 맞춰서 시도해야 한다.
            Texture2D* pTexture = pCache->GetResource<Texture2D>(name);
            SetTexture(unit, dynamic_cast<Texture*>(pTexture));
        }
    }

    // 아직 테크닉 설정 외엔 사용하는 부분이 없다.
    void Material::ResetToDefault()
    {
        // 임시
        SetName("DefaultMat");

        // 여기도 vertex, pixel defeines가 존재하네...

        Renderer* pRenderer = GetSubsystem<Renderer>();
        // 추후 좀 바꿔야 한다.
        // => 캐시의 GetResource에 파일을 전달해야 한다는 말이다.
        if (!pRenderer)
        {
            auto pTech = new Technique(m_pContext);
            pTech->SetName("DefaultTech");
            auto pPass = pTech->CreatePass("base");

            // 글로벌과 개별 섀이더로 나뉘지만 일단은 하나만 사용
            // 그리고 urho는 확장자도 없이 이름만 저장된다.
            // 그려면 Graphics에서 로드할 때 경로를 직접 붙인다.
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


        // shader parameter도 설정한다.

        // cull mode
        // fill mode
    }
}