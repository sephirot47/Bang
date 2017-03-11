#include "Texture2DAssetFile.h"

#include "stb_image.h"

#include "Debug.h"
#include "XMLNode.h"
#include "FileReader.h"
#include "IO.h"

#ifdef BANG_EDITOR
#include "IInspectable.h"
#include "Texture2DAssetFileInspectable.h"
#endif

Texture2DAssetFile::Texture2DAssetFile
    (const QFileSystemModel *model, const QModelIndex &index) : File(model, index)
{
}

String Texture2DAssetFile::GetImageAbsFilepath() const
{
    String contents = IO::GetFileContents(GetAbsolutePath());
    XMLNode *xmlInfo = XMLNode::FromString(contents);
    String result = "";
    if (xmlInfo)
    {
        result = xmlInfo->GetFilepath("ImageFilepath");
        delete xmlInfo;
    }
    return result;
}

QPixmap Texture2DAssetFile::GetIcon() const
{
    String fp = GetImageAbsFilepath();

    static Map<std::string, QPixmap> filepath_To_Pixmap;
    if (!filepath_To_Pixmap.ContainsKey(fp))
    {
        QPixmap pm(fp.ToQString());
        filepath_To_Pixmap[fp] = pm;
    }

    return filepath_To_Pixmap[fp];
}

#ifdef BANG_EDITOR
IInspectable *Texture2DAssetFile::GetInspectable() const
{
    return new Texture2DAssetFileInspectable(*this);
}
#endif

bool Texture2DAssetFile::IsAsset() const
{
    return true;
}
