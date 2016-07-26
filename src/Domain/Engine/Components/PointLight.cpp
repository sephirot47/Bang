#include "PointLight.h"

PointLight::PointLight() : Light()
{
    m_lightMaterial = AssetsManager::GetAsset<Material>("Assets/Engine/Materials/PR_PointLight.bmat");

    #ifdef BANG_EDITOR
    m_inspectorInfo.AddSlotInfos(
    {
        new InspectorVFloatSWInfo("Range", 1 )
    });
    #endif
}


void PointLight::SetUniformsBeforeApplyingLight() const
{
    Light::SetUniformsBeforeApplyingLight();

    ShaderProgram *sp = m_lightMaterial->GetShaderProgram();
    sp->SetUniformFloat("B_light_range", m_range, false);
}


const std::string PointLight::ToString() const
{
    return "PointLight";
}

std::string PointLight::GetName() const
{
    return "PointLight";
}

void PointLight::CloneInto(ICloneable *clone) const
{
    Light::CloneInto(clone);
    PointLight *pl = static_cast<PointLight*>(clone);
    pl->SetRange(GetRange());
}

ICloneable *PointLight::Clone() const
{
    PointLight *pl = new PointLight();
    CloneInto(pl);
    return pl;
}

#ifdef BANG_EDITOR
InspectorWidgetInfo *PointLight::OnInspectorInfoNeeded()
{
    Light::OnInspectorInfoNeeded();
    m_inspectorInfo.GetSlotInfo("Range")->SetFloatValue(m_range);
    return &m_inspectorInfo;
}

void PointLight::OnInspectorInfoChanged(InspectorWidgetInfo *info)
{
    Light::OnInspectorInfoChanged(info);
    m_range = info->GetFloat("Range");

}
#endif

void PointLight::SetRange(float range)
{
    m_range = range;
}

float PointLight::GetRange() const
{
    return m_range;
}

void PointLight::WriteInternal(std::ostream &f) const
{
    Light::WriteInternal(f);
    FileWriter::WriteFloat(m_range, f);
}

void PointLight::ReadInternal(std::istream &f)
{
    Light::ReadInternal(f);
    m_range = FileReader::ReadFloat(f);
}