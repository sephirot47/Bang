#ifndef LIGHT_H
#define LIGHT_H

#include "GBuffer.h"
#include "Component.h"

class Light : public Component
{
friend class Scene;

protected:
    Vector3 m_color;
    Material *m_material = nullptr;

    virtual void ApplyLight(GBuffer *gbuffer) const;

public:
    Light();

    virtual const std::string ToString() const override;
    virtual std::string GetName() const override { return "Directional Light"; }

    virtual void CloneInto(ICloneable *clone) const override;
    virtual ICloneable *Clone() const override;

    #ifdef BANG_EDITOR
    virtual InspectorWidgetInfo* GetComponentInfo() override;
    virtual void OnSlotValueChanged(InspectorWidget *source) override;
    #endif

    virtual void SetColor(const Vector3 &color);

    Vector3 GetColor() const;

    virtual void Write(std::ostream &f) const;
    virtual void Read(std::istream &f);
};

#endif // LIGHT_H
