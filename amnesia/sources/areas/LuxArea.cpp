#include "LuxArea.h"

#include "LuxMap.h"
#include "LuxMapHandler.h"
#include "LuxProp.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxAreaLoader::Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform, cWorld *apWorld)
{
    cLuxMap *pMap = gpBase->mpCurrentMapLoading;
    if(pMap == NULL)
    {
        return;
    }

    iLuxArea *pArea = CreateArea(asName, alID, pMap);

    //////////////////////////////
    // Create and set body
    iPhysicsWorld *pPhysicsWorld = apWorld->GetPhysicsWorld();
    iCollideShape* pShape = pPhysicsWorld->CreateBoxShape(avSize, NULL);
    iPhysicsBody* pBody = pPhysicsWorld->CreateBody(asName, pShape);

    pBody->SetCollide(false);
    pBody->SetCollideCharacter(false);
    pBody->SetMatrix(a_mtxTransform);
    pBody->SetUserData(pArea);

    pArea->mpBody = pBody;

    //////////////////////////////
    // Load base properties
    pArea->mvSize = avSize;
    pArea->m_mtxTransform = a_mtxTransform;

    //////////////////
    //Load variables
    LoadVariables(pArea, apWorld);

    //////////////////////////////
    // Load type specific properties
    SetupArea(pArea, apWorld);

    pMap->AddEntity(pArea);

    pArea->SetActive(abActive);

    pArea->SetupAfterLoad(apWorld);
}

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxArea::iLuxArea(const tString &asName, int alID, cLuxMap *apMap, eLuxAreaType aAreaType)  : iLuxEntity(asName, alID, apMap, eLuxEntityType_Area)
{
    mAreaType = aAreaType;

    mpBody = NULL;

    mpParentBody = NULL;
    mvRelativeOffset = 0;
}

//-----------------------------------------------------------------------

iLuxArea::~iLuxArea()
{
    if(mpBody)
    {
        mpMap->GetPhysicsWorld()->DestroyBody(mpBody);
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool iLuxArea::CanInteract(iPhysicsBody *apBody)
{
    return false;
}

//-----------------------------------------------------------------------

bool iLuxArea::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
    return false;
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair iLuxArea::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
    return eLuxFocusCrosshair_LastEnum;
}

//-----------------------------------------------------------------------

iEntity3D *iLuxArea::GetAttachEntity()
{
    return mpBody;
}

//-----------------------------------------------------------------------

void iLuxArea::AttachToBody(const tString& asPropName, int alBodyId)
{
    iLuxEntity *apEntity = mpMap->GetEntityByName(asPropName);

    if(apEntity->GetEntityType() == eLuxEntityType_Prop)
    {
        iLuxProp* pProp = static_cast<iLuxProp*>(apEntity);

        if( pProp->GetBodyNum() > 0 )
        {
            iPhysicsBody* pPropBody = pProp->GetBody(alBodyId);

            if(pPropBody)
            {
                msPropName = asPropName;
                mlBodyId = alBodyId;
                mpParentBody = pPropBody;
                mvRelativeOffset = mpBody->GetWorldPosition() - mpParentBody->GetWorldPosition();

                return;
            }
        }
    }

    /////////////
    // Failed
    DetachBody();
}

void iLuxArea::DetachBody()
{
    msPropName = "";
    mpParentBody = NULL;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

void iLuxArea::OnSetActive(bool abX)
{
    ///////////////
    //Bodies
    if(mpBody)
    {
        mpBody->SetActive(abX);
    }
}

//-----------------------------------------------------------------------

void iLuxArea::OnUpdate(double adFixedDelta)
{
    // Not currently being visited by LuxArea_Liquid due to having their own OnUpdate function?
    if(mpParentBody)
    {
        mpBody->StaticLinearMove(mvRelativeOffset + mpParentBody->GetWorldPosition() - mpBody->GetWorldPosition());
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeVirtual(iLuxArea_SaveData, iLuxEntity_SaveData)
kSerializeVar(mvSize, eSerializeType_Vector3f)
kSerializeVar(m_mtxTransform, eSerializeType_Matrixf)
kSerializeVar(mvRelativeOffset, eSerializeType_Vector3f)
kSerializeVar(msPropName, eSerializeType_String)
kSerializeVar(mlBodyId, eSerializeType_Int32)
kEndSerialize()


//-----------------------------------------------------------------------

iLuxEntity *iLuxArea_SaveData::CreateEntity(cLuxMap *apMap)
{
    iLuxArea *pArea = CreateArea(apMap);
    apMap->AddEntity(pArea);

    return pArea;
}

//-----------------------------------------------------------------------

void iLuxArea::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::SaveToSaveData(apSaveData);
    iLuxArea_SaveData *pData = static_cast<iLuxArea_SaveData *>(apSaveData);

    kCopyToVar(pData, mvSize);
    kCopyToVar(pData, m_mtxTransform);

    kCopyToVar(pData, mvRelativeOffset);
    kCopyToVar(pData, msPropName);
    kCopyToVar(pData, mlBodyId);
}

//-----------------------------------------------------------------------

void iLuxArea::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::LoadFromSaveData(apSaveData);
    iLuxArea_SaveData *pData = static_cast<iLuxArea_SaveData *>(apSaveData);

    kCopyFromVar(pData, mvSize);
    kCopyFromVar(pData, m_mtxTransform);

    kCopyFromVar(pData, mvRelativeOffset);
    kCopyFromVar(pData, msPropName);
    kCopyFromVar(pData, mlBodyId);

    //////////////////////////////
    // Create and set body
    iPhysicsWorld *pPhysicsWorld = mpMap->GetPhysicsWorld();
    iCollideShape* pShape = pPhysicsWorld->CreateBoxShape(mvSize, NULL);
    iPhysicsBody* pBody = pPhysicsWorld->CreateBody(msName, pShape);

    pBody->SetCollide(false);
    pBody->SetCollideCharacter(false);
    pBody->SetMatrix(m_mtxTransform);
    pBody->SetUserData(this);
    pBody->SetActive(mbActive);

    mpBody = pBody;

    ///////////////////
    //Do setup
    SetupAfterLoad(mpMap->GetWorld());
}

//-----------------------------------------------------------------------

void iLuxArea::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
    super_class::SetupSaveData(apSaveData);
    iLuxArea_SaveData *pData = static_cast<iLuxArea_SaveData *>(apSaveData);

    if(msPropName != "")
    {
        cVector3f vOffset = mvRelativeOffset;
        AttachToBody(msPropName, mlBodyId);
        mvRelativeOffset = vOffset;
    }
}

//-----------------------------------------------------------------------

