#ifndef INSPECTORTEXTURE2DWIDGET_H
#define INSPECTORTEXTURE2DWIDGET_H

#include "Bang/ImageFile.h"
#include "Bang/SerializableObject.h"

class ImageFileInspectable : public SerializableObject
{
    OBJECT(ImageFileInspectable)

private:
    ImageFile m_fileImage;

public:
    ImageFileInspectable();
    ImageFileInspectable(const ImageFile &fileImage);

    virtual const QPixmap& GetIcon() const override;

    virtual void Read(const XMLNode &xmlInfo) override;
    virtual void Write(XMLNode *xmlInfo) const override;
};

#endif // INSPECTORTEXTURE2DWIDGET_H
