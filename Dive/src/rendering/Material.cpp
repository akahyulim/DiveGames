#include "stdafx.h"
#include "Material.h"
#include "graphics/Texture2D.h"
#include "graphics/ShaderManager.h"
#include "graphics/ShaderProgram.h"
#include "graphics/ConstantBuffer.h"
#include "graphics/Buffer.h"
#include "resource/YamlHelper.h"
#include "resource/ResourceManager.h"

namespace Dive
{
    Material::Material(ID3D11Device* device)
    {
        memset(&m_cpuBuffer, 0, sizeof(MaterialData));
        m_gpuBuffer = std::make_unique<ConstantBuffer>(
            device, 
            ePSConstantBufferSlot::Material, 
            static_cast<uint32_t>(sizeof(MaterialData)));
        if (!m_gpuBuffer) DV_LOG(Material, err, "[::Material] ConstantBuffer 생성 실패");
        
        SetName("Standard");
    }

    bool Material::LoadFromFile(const std::filesystem::path& filepath)
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filepath.string());
        }
        catch (const YAML::ParserException& e)
        {
            DV_LOG(Material, err, "[::LoadFromFile] YAML LoadFile 실패: {}", e.what());
            return false;
        }

        if (data["Material"])
            SetName(data["Material"].as<std::string>());
       
        if (data["DiffuseMap"])     SetMap(eMapType::Diffuse, data["DiffuseMap"].as<std::string>());
        if (data["DiffuseColor"])   m_cpuBuffer.diffuseColor = data["DiffuseColor"].as<DirectX::XMFLOAT4>();
        if (data["Tiling"])         m_cpuBuffer.tiling = data["Tiling"].as<DirectX::XMFLOAT2>();
        if (data["Offset"])         m_cpuBuffer.offset = data["Offset"].as<DirectX::XMFLOAT2>();
        if (data["Propertices"])    m_cpuBuffer.propertices = data["Propertices"].as<uint32_t>();

        return true;
    }

    bool Material::SaveToFile(const std::filesystem::path& filepath)
    {
        const std::string& diffuseTexture = m_maps[static_cast<uint8_t>(eMapType::Diffuse)] ?
			m_maps[static_cast<uint8_t>(eMapType::Diffuse)]->GetName() : "";

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Material" << YAML::Value << GetName();

        out << YAML::Key << "DiffuseMap" << YAML::Value << diffuseTexture;
        out << YAML::Key << "DiffuseColor" << YAML::Value << m_cpuBuffer.diffuseColor;
        out << YAML::Key << "Tiling" << YAML::Value << m_cpuBuffer.tiling;
        out << YAML::Key << "Offset" << YAML::Value << m_cpuBuffer.offset;
        out << YAML::Key << "Propertices" << YAML::Value << m_cpuBuffer.propertices;

        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();

        SetFilepath(filepath);

        return true;
    }

    void Material::Bind(ID3D11DeviceContext* deviceContext)
    {
        assert(deviceContext);

        ShaderManager::BindInputLayout("Unlit_VS", eInputLayout::Lit, deviceContext);
        ShaderManager::GetShader("Unlit_VS")->Bind(deviceContext);
        ShaderManager::GetShader("Unlit_PS")->Bind(deviceContext);

        //if (m_shaders)
        //    m_shaders->Bind(deviceContext);

        // shader resources
        if (m_maps[static_cast<size_t>(eMapType::Diffuse)])
            m_maps[static_cast<size_t>(eMapType::Diffuse)]->Bind(deviceContext, eShaderResourceSlot::DiffuseMap);

        // constant buffer
        m_gpuBuffer->Update<MaterialData>(deviceContext, m_cpuBuffer);
        m_gpuBuffer->Bind(deviceContext);
    }

    Texture2D* Material::GetMap(eMapType type)
    {
        if (type == eMapType::Count)
        {
            DV_LOG(Material, err, "[::GetMap] 잘못된 MapType 전달");
            return nullptr;
        }

		return m_maps[static_cast<size_t>(type)];
    }

    void Material::SetMap(eMapType type, Texture2D* texture)
    {
        if (type == eMapType::Count)
        {
            DV_LOG(Material, err, "[::SetMap] 잘못된 MapType 전달");
            return;
        }

        m_maps[static_cast<size_t>(type)] = texture;

        switch (type)
        {
        case eMapType::Diffuse:
            m_cpuBuffer.propertices = m_maps[static_cast<size_t>(type)] ? (1U << 0) : 0;
            break;
        case eMapType::Normal:
            m_cpuBuffer.propertices = m_maps[static_cast<size_t>(type)] ? (1U << 1) : 0;
            break;
        default:
            break;
        }

    }

    void Material::SetMap(eMapType type, const std::string& textureName)
    {
        if (type == eMapType::Count)
        {
            DV_LOG(Material, err, "[::SetMap] 잘못된 MapType 전달");
            return;
        }

		auto mapTexture = ResourceManager::GetByName<Texture2D>(textureName);
        m_maps[static_cast<size_t>(type)] = mapTexture;

        switch (type)
        {
        case eMapType::Diffuse:
            m_cpuBuffer.propertices |= mapTexture ? (1U << 0) : 0;
            break;
        case eMapType::Normal:
            m_cpuBuffer.propertices |= mapTexture ? (1U << 1) : 0;
            break;
        default:
            break;
        }
    }

    void Material::SetMap(eMapType type, const std::filesystem::path& filepath)
    {
        if (type == eMapType::Count)
        {
            DV_LOG(Material, err, "[::SetMap] 잘못된 MapType 전달");
            return;
        }

        auto mapTexture = ResourceManager::GetOrLoadByPath<Texture2D>(filepath);
        m_maps[static_cast<size_t>(type)] = mapTexture;

        switch (type)
        {
        case eMapType::Diffuse:
            m_cpuBuffer.propertices |= mapTexture ? (1U << 0) : 0;
            break;
        case eMapType::Normal:
            m_cpuBuffer.propertices |= mapTexture ? (1U << 1) : 0;
            break;
        default:
            break;
        }
    }

    bool Material::IsTransparent() const
    {
        return m_blendMode != eBlendMode::Opqaue;
    }
}
