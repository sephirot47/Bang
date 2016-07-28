#include "FileWriter.h"
#include "Scene.h"
#include "Asset.h"

FileWriter::FileWriter()
{
}

void FileWriter::WriteScene(std::string filename, Scene *scene)
{
    std::ofstream ofs;
    filename = Persistence::AppendExtension(filename, Scene::GetFileExtension());
    ofs.open(filename);
    if (!ofs.is_open())
    {
        Logger_Error("There was an error when saving file '" << filename << "'");
        ofs.close();
        return;
    }

    XMLNode *xmlInfo = new XMLNode();
    scene->FillXMLInfo(xmlInfo);
    ofs << xmlInfo->ToString();
    ofs.close();
}

void FileWriter::WriteAsset(std::string filename, Asset *a)
{
    std::ofstream ofs;
    filename = Persistence::AppendExtension(filename,
                                            a->GetFileExtension());
    ofs.open(filename);
    if (!ofs.is_open())
    {
        Logger_Error("There was an error when saving file '" << filename << "'");
        ofs.close();
        return;
    }

    XMLNode *xmlInfo = new XMLNode();
    a->FillXMLInfo(xmlInfo);
    ofs << xmlInfo->ToString();
    ofs.close();
}

void FileWriter::WriteXMLNode(XMLNode *xmlInfo, std::ostream &f)
{
    NONULL(xmlInfo);
    f << xmlInfo->ToString();
}

void FileWriter::WriteBool(bool v, std::ostream &f)
{
    std::string s = v ? "true" : "false";
    f << s << std::endl;
}

void FileWriter::WriteInt(int v, std::ostream &f)
{
    f << v << std::endl;
}

void FileWriter::WriteFloat(float v, std::ostream &f)
{
    f << v << std::endl;
}

void FileWriter::WritePointer(void *v, std::ostream &f)
{
    f << v << std::endl;
}

void FileWriter::WriteVector2(const glm::vec2 &v, std::ostream &f)
{
    f << v.x << " " << v.y << std::endl;
}

void FileWriter::WriteVector3(const Vector3 &v, std::ostream &f)
{
    f << v.x << " " << v.y << " " << v.z << std::endl;
}

void FileWriter::WriteVector4(const glm::vec4 &v, std::ostream &f)
{
    f << v.x << " " << v.y << " " << v.z << " " << v.w << std::endl;
}


void FileWriter::WriteQuaternion(const Quaternion &q, std::ostream &f)
{
    f << q.x << " " << q.y << " " << q.z << " " << q.w << std::endl;
}

void FileWriter::WriteRect(const Rect &r, std::ostream &f)
{
    f << r.m_minx << " " << r.m_maxx << " " << r.m_miny << " " << r.m_maxy << std::endl;
}

void FileWriter::WriteString(const std::string &str, std::ostream &f)
{
    if (str != "")
    {
        f << str << std::endl;
    }
    else
    {
        f << "-" << std::endl;
    }
}

void FileWriter::WriteFilepath(const std::string &path, std::ostream &f)
{
    FileWriter::WriteString(Persistence::ProjectRootAbsoluteToRelative(path), f);
}
