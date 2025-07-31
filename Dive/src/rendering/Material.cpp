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
        assert(device);

        m_gpuBuffer = std::make_unique<ConstantBuffer>(
            device, 
            ePSConstantBufferSlot::Material, 
            static_cast<uint32_t>(sizeof(MaterialConstants)));
        if (!m_gpuBuffer) DV_LOG(Material, err, "[::Material] ConstantBuffer 생성 실패");

        m_cpuBuffer.diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        m_cpuBuffer.tiling = { 1.0f, 1.0f };
        m_cpuBuffer.offset = { 0.0f, 0.0f };

        m_isDirty = true;

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
            DV_LOG(Material, warn, "YAML 파싱 실패: {}", e.what());
            return false;
        }

        if (data["Material"])
            SetName(data["Material"].as<std::string>());

        // 셰이더 이름을 읽어야 한다.
       
        if (data["DiffuseTexture"])
            SetTexture(eTextureUnitType::Diffuse, data["DiffuseTexture"].as<std::string>());
      
        if (data["DiffuseColor"])
            m_cpuBuffer.diffuseColor = data["DiffuseColor"].as<DirectX::XMFLOAT4>();

        if (data["Tiling"])
            m_cpuBuffer.tiling = data["Tiling"].as<DirectX::XMFLOAT2>();

        if (data["Offset"])
            m_cpuBuffer.offset = data["Offset"].as<DirectX::XMFLOAT2>();

        return true;
    }

    bool Material::SaveToFile(const std::filesystem::path& filepath)
    {
        const std::string& diffuseTexture = m_textures[static_cast<uint8_t>(eTextureUnitType::Diffuse)] ?
			m_textures[static_cast<uint8_t>(eTextureUnitType::Diffuse)]->GetName() : "";

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Material" << YAML::Value << GetName();

        // 셰이더 이름을 저장해야 한다.
        out << YAML::Key << "DiffuseTexture" << YAML::Value << diffuseTexture;
        out << YAML::Key << "DiffuseColor" << YAML::Value << m_cpuBuffer.diffuseColor;
        out << YAML::Key << "Tiling" << YAML::Value << m_cpuBuffer.tiling;
        out << YAML::Key << "Offset" << YAML::Value << m_cpuBuffer.offset;

        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();

        SetFilepath(filepath);

        return true;
    }

    void Material::Bind(ID3D11DeviceContext* deviceContext)
    {
        assert(deviceContext);

        ShaderManager::GetShader("Unlit_VS")->Bind(deviceContext);
        ShaderManager::GetShader("Unlit_PS")->Bind(deviceContext);
        ShaderManager::BindInputLayout("Unlit_VS", eInputLayout::Lit, deviceContext);

        // 결국엔 이걸 사용해야 할 거다.
        //if (m_shaders)
        //    m_shaders->Bind(deviceContext);

        // constant buffer
        MaterialConstants data{};
        data.diffuseColor = GetDiffuseColor();
        data.offset = GetOffset();
        data.tiling = GetTiling();
        m_gpuBuffer->Update<MaterialConstants>(deviceContext, m_cpuBuffer);
        m_gpuBuffer->Bind(deviceContext);

        // shader resources
    }

    std::shared_ptr<Texture2D> Material::GetTexture(eTextureUnitType type)
    {
        assert(type != eTextureUnitType::None);
		return m_textures[static_cast<uint8_t>(type)];
    }

    void Material::SetTexture(eTextureUnitType type, std::shared_ptr<Texture2D> texture)
    {
		assert(type != eTextureUnitType::None);

        m_textures[static_cast<uint8_t>(type)] = texture;
        
        m_isDirty = true;
    }

    void Material::SetTexture(eTextureUnitType type, const std::string& textureName)
    {
        assert(type != eTextureUnitType::None);

		auto diffuseTex = ResourceManager::GetByName<Texture2D>(textureName);
		m_textures[static_cast<uint8_t>(type)] = diffuseTex;

		m_isDirty = true;
    }

    bool Material::IsTransparent() const
    {
        return m_blendMode != eBlendMode::Opqaue;
    }
}
