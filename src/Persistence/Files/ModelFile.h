#ifndef MODELFILE_H
#define MODELFILE_H

#include <QPixmap>
#include "Bang/WinUndef.h"

#include "Bang/File.h"

class SerializableObject;
class ModelFile : public File
{
public:
    ModelFile();
    ModelFile(const Path& path);

    void Read(const XMLNode &xmlInfo) override;
    void Write(XMLNode *xmlInfo) const override;

    virtual const QPixmap& GetIcon() const override;

    #ifdef BANG_EDITOR
    virtual IInspectable *GetNewInspectable() override;
    #endif

    virtual bool IsAsset() const override;

    bool IsTriangles() const;
    int GetNumFaces() const;

private:
    bool m_isTris = true;
    int m_numFaces = 0;
};

#endif // MODELFILE_H
