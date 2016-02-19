#ifndef ASSETSREADER_H
#define ASSETSREADER_H

#include "ReaderWithPointerIds.h"
#include "Asset.h"

class AssetsReader
{
private:
    AssetsReader() {}

public:

    //Reads the file "<assets> ... </assets>"
    static void ReadAssetsFile(const std::string &filepath);


    //The input must not have the opening tag!
    template <class T>
    static T* ReadAsset(std::ifstream &f)
    {
        Asset *a = new T();
        a->Read(f);
        FileReader::ReadNextLine(f); //Consume close tag
        return dynamic_cast<T*>(a);
    }


    //Reads a specific asset file (image.png, myMaterial.mat, etc.)
    template <class T>
    static T* ReadAssetFile(const std::string &filepath)
    {
        std::ifstream f (filepath);
        if ( !f.is_open() )
        {
            Logger_Error("Could not open the file '" << filepath << "' to load this asset.");
            return nullptr;
        }

        //Skip open tag
        return ReadAsset<T>(f);
    }
};

#endif // ASSETSREADER_H
