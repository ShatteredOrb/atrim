#ifndef LUX_ITEM_TYPE_H
#define LUX_ITEM_TYPE_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxInventory_Item;

//----------------------------------------------

class iLuxItemType
{
public:
    iLuxItemType(const tString& asName, eLuxItemType aType);
    ~iLuxItemType();

    const tString &GetName()
    {
        return msName;
    }
    eLuxItemType GetType()
    {
        return mType;
    }

    bool HasCount()
    {
        return mbHasCount;
    }
    int GetMaxCount()
    {
        return mlMaxCount;
    }
    bool ShowPickUpMessage()
    {
        return mbShowPickUpMessage;
    }

    virtual bool GetHasMaxAmount()
    {
        return false;
    }
    virtual void OnUse(cLuxInventory_Item *apItem, int alSlotIndex) = 0;
    virtual tWString GetDisplayedNameAdd(cLuxInventory_Item *apItem)
    {
        return _W("");
    }

    virtual bool BeforeAddItem(cLuxInventory_Item *apItem)
    {
        return false;
    }

protected:
    void AddCompletionAmount(int alAmount);
    bool mbHasCount;
    int mlMaxCount;
    bool mbShowPickUpMessage;

private:
    tString msName;
    eLuxItemType mType;

};

//----------------------------------------------


class cLuxItemType_Puzzle : public iLuxItemType
{
public:
    cLuxItemType_Puzzle();

    bool BeforeAddItem(cLuxInventory_Item *apItem);
    void OnUse(cLuxInventory_Item *apItem, int alSlotIndex);

private:
};

//----------------------------------------------

class cLuxItemType_Coins : public iLuxItemType
{
public:
    cLuxItemType_Coins();

    bool BeforeAddItem(cLuxInventory_Item *apItem);
    void OnUse(cLuxInventory_Item *apItem, int alSlotIndex);

private:
};

//----------------------------------------------

class cLuxItemType_Note : public iLuxItemType
{
public:
    cLuxItemType_Note();

    bool BeforeAddItem(cLuxInventory_Item *apItem);
    void OnUse(cLuxInventory_Item *apItem, int alSlotIndex);

private:
};

//----------------------------------------------

class cLuxItemType_Diary : public iLuxItemType
{
public:
    cLuxItemType_Diary();

    bool BeforeAddItem(cLuxInventory_Item *apItem);
    void OnUse(cLuxInventory_Item *apItem, int alSlotIndex);

    static bool mbShowJournalOnPickup;
private:
};

//----------------------------------------------

class cLuxItemType_Lantern : public iLuxItemType
{
public:
    cLuxItemType_Lantern();

    bool BeforeAddItem(cLuxInventory_Item *apItem);
    void OnUse(cLuxInventory_Item *apItem, int alSlotIndex);
    tWString GetDisplayedNameAdd(cLuxInventory_Item *apItem);


private:
};

//----------------------------------------------

class cLuxItemType_Health : public iLuxItemType
{
public:
    cLuxItemType_Health();

    bool BeforeAddItem(cLuxInventory_Item *apItem);
    void OnUse(cLuxInventory_Item *apItem, int alSlotIndex);

private:
};

//----------------------------------------------

class cLuxItemType_Sanity : public iLuxItemType
{
public:
    cLuxItemType_Sanity();

    bool BeforeAddItem(cLuxInventory_Item *apItem);
    void OnUse(cLuxInventory_Item *apItem, int alSlotIndex);

private:
};

//----------------------------------------------

class cLuxItemType_LampOil : public iLuxItemType
{
public:
    cLuxItemType_LampOil();

    bool BeforeAddItem(cLuxInventory_Item *apItem);
    void OnUse(cLuxInventory_Item *apItem, int alSlotIndex);

private:
};

//----------------------------------------------

class cLuxItemType_Tinderbox : public iLuxItemType
{
public:
    cLuxItemType_Tinderbox();

    bool BeforeAddItem(cLuxInventory_Item *apItem);
    void OnUse(cLuxInventory_Item *apItem, int alSlotIndex);

private:
};

//----------------------------------------------


class cLuxItemType_HandObject : public iLuxItemType
{
public:
    cLuxItemType_HandObject();

    bool BeforeAddItem(cLuxInventory_Item *apItem);

    void OnUse(cLuxInventory_Item *apItem, int alSlotIndex);

private:
};


//----------------------------------------------

#endif // LUX_ITEM_TYPE_H
