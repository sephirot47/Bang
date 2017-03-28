#ifndef ASSETSMANAGER_H
#define ASSETSMANAGER_H

#include <sstream>

#include "Bang/Map.h"
#include "Bang/Debug.h"
#include "Bang/Asset.h"
#include "Bang/Object.h"
#include "Bang/XMLParser.h"
#include "Bang/IO.h"
#include "Bang/BehaviourManager.h"

class AssetsManager
{
public:

    /** For every id, it contains a pointer to the asset
      * created when the assets were loaded.
    **/
    Map<String, Asset*> m_id_To_AssetPointer;

    AssetsManager();
    virtual ~AssetsManager();

    static bool IsLoaded(const String &filepath, bool isEngineAsset = false);

    /** This function prepares a relative path, to be searched by
     *   a loading function. Or to be saved to the AssetsManager cache.
    **/
    static String FormatFilepath(const String &filepath,
                                 bool isEngineFilepath);

    template <class T>
    static T* GetAsset(const String &filepath, bool isEngineAsset)
    {
        AssetsManager *am = AssetsManager::GetCurrent();
        if (!AssetsManager::IsLoaded(filepath, isEngineAsset))
        {
            return nullptr;
        }
        else
        {
            String f = AssetsManager::FormatFilepath(filepath, isEngineAsset);
            return Object::Cast<T>(am->m_id_To_AssetPointer[f]);
        }
    }

    /** Reads a specific asset file (btex2d, bmesh, etc.)
      * from a filepath.
    **/
    template <class T>
    static T* ReadAssetFile(const String &filepath, bool isEngineAsset)
    {
        T *a = nullptr;
        String absPath = AssetsManager::FormatFilepath(filepath, isEngineAsset);
        if(!IO::ExistsFile(absPath)) { return nullptr; }

        XMLNode *xmlInfo = XMLParser::FromFile(absPath);
        if (xmlInfo)
        {
            a = new T();
            a->Read(*xmlInfo);
            a->m_assetFilepath = absPath;
            delete xmlInfo;
        }
        return a;
    }

    /** Tries to retrieve an Asset from the AssetsManager cache
      * If there's no such Asset, it is loaded, added to the cache, and returned
      * If the file can't be read, nullptr is returned  **/
    template <class T>
    static T* Create(const String &filepath,
                     bool isEngineAsset = false)
    {
        T *a = new T();
        a->m_assetFilepath = filepath;
        AssetsManager::SaveAssetToFile(filepath, a, isEngineAsset);
        return a;
    }

    /** Tries to retrieve an Asset from the AssetsManager cache
      * If there's no such Asset, it is loaded, added to the cache, and returned
      * If the file can't be read, nullptr is returned  **/
    template <class T>
    static T* Load(const String &filepath, bool isEngineAsset = false)
    {
        T *a = nullptr;
        if (!AssetsManager::IsLoaded(filepath, isEngineAsset))
        {
            a = AssetsManager::ReadAssetFile<T>(filepath, isEngineAsset);
            AssetsManager::SaveAssetToMap(filepath, a, isEngineAsset);
        }
        else
        {
            a = AssetsManager::GetAsset<T>(filepath, isEngineAsset);
        }
        return a;
    }

    #ifdef BANG_EDITOR
    template <class T>
    static void OnAssetFileChanged(const String &assetFilepath,
                                   const XMLNode &xmlChangedInfo)
    {
        // Update live instances and rewrite the file
        ASSERT(!assetFilepath.Empty());
        if (AssetsManager::IsLoaded(assetFilepath, false))
        {
            Asset *asset = AssetsManager::GetAsset<T>(assetFilepath, false);
            asset->Read(xmlChangedInfo);
        }

        if (IO::ExistsFile(assetFilepath))
        {
            IO::WriteToFile(assetFilepath, xmlChangedInfo.ToString());
        }
    }
    #endif

    static void SaveAssetToMap(const String &filepath, Asset* asset,
                               bool isEngineAsset);
    static void SaveAssetToFile(const String &filepath, Asset* asset,
                                bool isEngineAsset);

    static void Unload(const String &id);
    static void Unload(Asset *asset);

    static AssetsManager* GetCurrent();

    friend class Asset;
};

#endif // ASSETSMANAGER_H
