#include "stdafx.h"
#include "Material.h"
#include "graphics/Texture2D.h"
#include "graphics/ShaderManager.h"
#include "graphics/ConstantBuffer.h"
#include "core/CoreDefs.h"
#include "resource/YamlHelper.h"
#include "resource/ResourceManager.h"

namespace Dive
{
    Material::Material()
    {
        m_GpuBuffer = std::make_unique<ConstantBuffer>();
        m_GpuBuffer->Create(sizeof(MaterialConstants));

        m_ConstantBuffer = std::make_unique<DvConstantBuffer>(Graphics::GetDevice(), ePSConstantBufferSlot::PerMaterialPS, sizeof(MaterialConstants));

        m_CpuBuffer.diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        m_CpuBuffer.tiling = { 1.0f, 1.0f };
        m_CpuBuffer.offset = { 0.0f, 0.0f };

        m_Dirty = true;

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

        if (data["PixelShader"])
            m_PixelShaderName = data["PixelShader"].as<std::string>();

        if (data["DiffuseTexture"])
            SetTexture(eTextureUnitType::Diffuse, data["DiffuseTexture"].as<std::string>());
      
        if (data["DiffuseColor"])
            m_CpuBuffer.diffuseColor = data["DiffuseColor"].as<DirectX::XMFLOAT4>();

        if (data["Tiling"])
            m_CpuBuffer.tiling = data["Tiling"].as<DirectX::XMFLOAT2>();

        if (data["Offset"])
            m_CpuBuffer.offset = data["Offset"].as<DirectX::XMFLOAT2>();

        return true;
    }

    bool Material::SaveToFile(const std::filesystem::path& filepath)
    {
        const std::string& diffuseTexture = m_Textures[static_cast<UINT8>(eTextureUnitType::Diffuse)] ?
			m_Textures[static_cast<UINT8>(eTextureUnitType::Diffuse)]->GetName() : "";

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Material" << YAML::Value << GetName();

        out << YAML::Key << "PixelShader" << YAML::Value << m_PixelShaderName;
        out << YAML::Key << "DiffuseTexture" << YAML::Value << diffuseTexture;
        out << YAML::Key << "DiffuseColor" << YAML::Value << m_CpuBuffer.diffuseColor;
        out << YAML::Key << "Tiling" << YAML::Value << m_CpuBuffer.tiling;
        out << YAML::Key << "Offset" << YAML::Value << m_CpuBuffer.offset;

        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();

        SetFilepath(filepath);

        return true;
    }

    void Material::Bind(ID3D11DeviceContext* deviceContext)
    {
        assert(deviceContext);

        // vertex shader
        ID3D11VertexShader* oldVS = nullptr;
        deviceContext->VSGetShader(&oldVS, nullptr, nullptr);
        auto currentVS = ShaderManager::GetShader("Default_VS")->GetVertexShader();
        if (oldVS != currentVS)
            deviceContext->VSSetShader(currentVS, nullptr, 0);

        auto inputLayout = ShaderManager::GetShader("Default_VS")->GetInputLayout();
        deviceContext->IASetInputLayout(inputLayout);

        // pixel shader
        ID3D11PixelShader* oldPS = nullptr;
        deviceContext->PSGetShader(&oldPS, nullptr, nullptr);
        auto currentPS = ShaderManager::GetShader(m_PixelShaderName)->GetPixelShader();
        if (oldPS != currentPS)
            deviceContext->PSSetShader(currentPS, nullptr, 0);

        // constant buffer
        MaterialConstants data{};
        data.diffuseColor = GetDiffuseColor();
        data.offset = GetOffset();
        data.tiling = GetTiling();
        m_ConstantBuffer->Update<MaterialConstants>(deviceContext, m_CpuBuffer);
        m_ConstantBuffer->Bind(deviceContext);

        // shader resources
    }

    std::shared_ptr<Texture2D> Material::GetTexture(eTextureUnitType type)
    {
        assert(type != eTextureUnitType::None);
		return m_Textures[static_cast<UINT8>(type)];
    }

    void Material::SetTexture(eTextureUnitType type, std::shared_ptr<Texture2D> texture)
    {
		assert(type != eTextureUnitType::None);

        m_Textures[static_cast<UINT8>(type)] = texture;
        
        m_Dirty = true;
    }

    void Material::SetTexture(eTextureUnitType type, const std::string& textureName)
    {
        assert(type != eTextureUnitType::None);

		auto diffuseTex = ResourceManager::GetByName<Texture2D>(textureName);
		m_Textures[static_cast<UINT8>(type)] = diffuseTex;

		m_Dirty = true;
    }

    Shader* Material::GetPixelShader() const
    {
        auto shader = ShaderManager::GetShader(m_PixelShaderName);
		if (!shader || shader->GetType() != eShaderType::Pixel)
		{
			DV_LOG(Material, err, "픽셀 셰이더 획득 실패: {}", m_PixelShaderName);
			return nullptr;
		}

        return shader;
    }

    ConstantBuffer* Material::GetConstantBuffer()
    {
        if (m_Dirty)
        {
            auto data = static_cast<MaterialConstants*>(m_GpuBuffer->Map());
            data->diffuseColor = m_CpuBuffer.diffuseColor;
            data->tiling = m_CpuBuffer.tiling;
            data->offset = m_CpuBuffer.offset;
            m_GpuBuffer->Unmap();

            m_Dirty = false;
        }

        return m_GpuBuffer.get();
    }

    bool Material::IsTransparent() const
    {
        return m_BlendMode != eBlendMode::Opqaue;
    }
}
