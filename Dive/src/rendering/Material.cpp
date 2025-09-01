#include "stdafx.h"
#include "Material.h"
#include "graphics/Graphics.h"
#include "graphics/Texture2D.h"
#include "graphics/ShaderManager.h"
#include "graphics/Buffer.h"
#include "resource/YamlHelper.h"
#include "resource/ResourceManager.h"

namespace Dive
{
    Material::Material()
    {
        m_gpuBuffer = std::make_unique<ConstantBuffer>(
            ePSConstantBufferSlot::Material, 
            static_cast<uint32_t>(sizeof(MaterialData)));
        if (!m_gpuBuffer) DV_LOG(Material, err, "[::Material] ConstantBuffer 생성 실패");

        m_maps.fill(nullptr);
        
        SetName("Standard");

        m_shaderProgram = ShaderManager::GetProgram("ForwardLight");
    }

    bool Material::LoadFromFile(const std::filesystem::path& filepath)
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filepath.string());
        }
        catch (const YAML::BadFile& e) 
        {
            DV_LOG(Material, err, "[::LoadFromFile] 파일 열기 실패: {}", e.what());
            return false;
        }
        catch (const YAML::ParserException& e)
        {
            DV_LOG(Material, err, "[::LoadFromFile] YAML LoadFile 실패: {}", e.what());
            return false;
        }

        if (data["Material"])       SetName(data["Material"].as<std::string>());
       
        if (data["DiffuseMap"])     SetMap(eMapType::Diffuse, data["DiffuseMap"].as<std::string>());
        if (data["NormalMap"])      SetMap(eMapType::Normal, data["NormalMap"].as<std::string>()); 
        if (data["DiffuseColor"])   m_cpuBuffer.diffuseColor = data["DiffuseColor"].as<DirectX::XMFLOAT4>();
        if (data["Tiling"])         m_cpuBuffer.tiling = data["Tiling"].as<DirectX::XMFLOAT2>();
        if (data["Offset"])         m_cpuBuffer.offset = data["Offset"].as<DirectX::XMFLOAT2>();

        if (data["Shader"])         m_shaderProgram = ShaderManager::GetProgram(data["Shader"].as<std::string>());

        return true;
    }

    bool Material::SaveToFile(const std::filesystem::path& filepath)
    {
        const std::string& diffuseTexture = m_maps[static_cast<uint8_t>(eMapType::Diffuse)] ?
			m_maps[static_cast<uint8_t>(eMapType::Diffuse)]->GetName() : "";
        const std::string& normalTexture = m_maps[static_cast<uint8_t>(eMapType::Normal)] ?
            m_maps[static_cast<uint8_t>(eMapType::Normal)]->GetName() : "";

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Material" << YAML::Value << GetName();

        out << YAML::Key << "DiffuseMap" << YAML::Value << diffuseTexture;
        out << YAML::Key << "NormalMap" << YAML::Value << normalTexture;
        out << YAML::Key << "DiffuseColor" << YAML::Value << m_cpuBuffer.diffuseColor;
        out << YAML::Key << "Tiling" << YAML::Value << m_cpuBuffer.tiling;
        out << YAML::Key << "Offset" << YAML::Value << m_cpuBuffer.offset;

        out << YAML::Key << "Shader" << YAML::Value << m_shaderProgram->GetName();

        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();

        SetFilepath(filepath);

        return true;
    }
    
    void Material::Bind()
    {
        if (!m_shaderProgram)
            return;
        m_shaderProgram->Bind();

        // shader resources
        if (m_maps[static_cast<size_t>(eMapType::Diffuse)])
            m_maps[static_cast<size_t>(eMapType::Diffuse)]->Bind(eShaderResourceSlot::DiffuseMap);
        if (m_maps[static_cast<size_t>(eMapType::Normal)])
            m_maps[static_cast<size_t>(eMapType::Normal)]->Bind(eShaderResourceSlot::NormalMap);

        // constant buffer
        m_gpuBuffer->Update<MaterialData>(m_cpuBuffer);
        m_gpuBuffer->Bind();
    }

    std::shared_ptr<Texture2D> Material::GetMap(eMapType type)
    {
        if (type == eMapType::Count)
        {
            DV_LOG(Material, err, "[::GetMap] 잘못된 맵 타입");
            return nullptr;
        }

		return m_maps[static_cast<size_t>(type)];
    }

    void Material::SetMap(eMapType type, std::shared_ptr<Texture2D> texture)
    {
        if (type == eMapType::Count)
        {
            DV_LOG(Material, err, "[::SetMap] 잘못된 맵 타입");
            return;
        }

        m_maps[static_cast<size_t>(type)] = texture;

        switch (type)
        {
        case eMapType::Diffuse:
            m_cpuBuffer.textureMask = m_maps[static_cast<size_t>(type)] ? (1U << 0) : 0;
            break;
        case eMapType::Normal:
            m_cpuBuffer.textureMask = m_maps[static_cast<size_t>(type)] ? (1U << 1) : 0;
            break;
        default:
            break;
        }

    }

    void Material::SetMap(eMapType type, const std::string& textureName)
    {
        if (type == eMapType::Count)
        {
            DV_LOG(Material, err, "[::SetMap] 잘못된 맵 타입");
            return;
        }

		auto mapTexture = ResourceManager::GetByName<Texture2D>(textureName);
        m_maps[static_cast<size_t>(type)] = mapTexture;

        switch (type)
        {
        case eMapType::Diffuse:
            m_cpuBuffer.textureMask |= mapTexture ? (1U << 0) : 0;
            break;
        case eMapType::Normal:
            m_cpuBuffer.textureMask |= mapTexture ? (1U << 1) : 0;
            break;
        default:
            break;
        }
    }

    void Material::SetMap(eMapType type, const std::filesystem::path& filepath)
    {
        if (type == eMapType::Count)
        {
            DV_LOG(Material, err, "[::SetMap] 잘못된 맵 타입");
            return;
        }

        auto mapTexture = ResourceManager::GetOrLoad<Texture2D>(filepath);
        m_maps[static_cast<size_t>(type)] = mapTexture;

        switch (type)
        {
        case eMapType::Diffuse:
            m_cpuBuffer.textureMask |= mapTexture ? (1U << 0) : 0;
            break;
        case eMapType::Normal:
            m_cpuBuffer.textureMask |= mapTexture ? (1U << 1) : 0;
            break;
        default:
            break;
        }
    }

    std::string Material::GetShaderProgramName() const
    {
        if(!m_shaderProgram)
            return std::string();

        return m_shaderProgram->GetName();
    }

    void Material::SetShaderProgramByName(const std::string& name)
    {
        m_shaderProgram = ShaderManager::GetProgram(name);
    }

    bool Material::IsTransparent() const
    {
        return m_blendMode != eBlendMode::Opqaue;
    }
}
