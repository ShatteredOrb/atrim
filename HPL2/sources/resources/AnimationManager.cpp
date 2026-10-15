#include "resources/AnimationManager.h"
#include "system/String.h"
#include "system/System.h"
#include "resources/Resources.h"
#include "graphics/Mesh.h"
#include "graphics/Animation.h"
#include "system/LowLevelSystem.h"
#include "resources/MeshLoaderHandler.h"
#include "resources/FileSearcher.h"


namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cAnimationManager::cAnimationManager(cGraphics* apGraphic, cResources *apResources)
    : iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
                       apResources->GetLowLevelSystem())
{
    mpGraphics = apGraphic;
    mpResources = apResources;
}

cAnimationManager::~cAnimationManager()
{
    DestroyAll();

    Log(" Done with animations\n");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cAnimation *cAnimationManager::CreateAnimation(const tString& asName)
{
    tWString sPath;
    cAnimation *pAnimation = NULL;
    tString asNewName;

    BeginLoad(asName);

    asNewName = asName;

    //If the file is missing an extension, search for an existing file.
    if(cString::GetFileExt(asNewName) == "")
    {
        bool bFound = false;
        tStringVec *pTypes = mpResources->GetMeshLoaderHandler()->GetSupportedTypes();
        for(size_t i = 0; i < pTypes->size(); i++)
        {
            asNewName = cString::SetFileExt(asNewName, (*pTypes)[i]);
            tWString sPath = mpResources->GetFileSearcher()->GetFilePath(asNewName);
            if(sPath != _W(""))
            {
                bFound = true;
                break;
            }
        }

        if(bFound == false)
        {
            Error("Couldn't find animation file '%s' in any supported format!\n", asName.c_str());
            EndLoad();
            return NULL;
        }
    }

    pAnimation = static_cast<cAnimation *>(this->FindLoadedResource(asNewName, sPath));

    if(pAnimation == NULL && sPath != _W(""))
    {
        cMeshLoaderHandler *pMeshLoadHandler = mpResources->GetMeshLoaderHandler();
        pAnimation = pMeshLoadHandler->LoadAnimation(sPath);

        if(pAnimation)
        {
            AddResource(pAnimation);
        }
    }

    if(pAnimation)
    {
        pAnimation->IncUserCount();
    }
    else
    {
        Error("Couldn't create animation '%s'\n", asNewName.c_str());
    }

    EndLoad();
    return pAnimation;
}

//-----------------------------------------------------------------------

void cAnimationManager::Unload(iResourceBase* apResource)
{

}
//-----------------------------------------------------------------------

void cAnimationManager::Destroy(iResourceBase* apResource)
{
    apResource->DecUserCount();

    if(apResource->HasUsers() == false)
    {
        RemoveResource(apResource);
        hplDelete(apResource);
    }
}

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
}
