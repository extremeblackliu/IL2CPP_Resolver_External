#pragma once

namespace GameObject
{
    inline GameObjectManager GOM;
    inline GameObjectManager* GOM_Ptr;
    static void RefreshObject()
    {
        um::ReadMemory(GOM_Ptr, &GOM);
    }

    static CGameObject* Find(const char* m_Name)
    {
        RefreshObject();

        LastObjectBase lastTaggedObject;
        um::ReadMemory(GOM.lastTaggedObject, &lastTaggedObject);

        BaseObject TaggedObjects;
        um::ReadMemory(GOM.taggedObjects, &TaggedObjects);

        if ((uintptr_t)GOM.taggedObjects == (uintptr_t)GOM_Ptr)
        {
            return nullptr;
        }

        CGameObject* lastGameObject = lastTaggedObject.lastObject;

        CGameObject gameObject;
        um::ReadMemory(lastGameObject, &gameObject);

        char buf0[255];
        um::ReadMemory(gameObject.m_ObjectName, &buf0);
        if (!strcmp(buf0, m_Name))
        {
            return lastGameObject;
        }

        CGameObject* curGameObject = TaggedObjects.object;

        BaseObject curBaseObject = TaggedObjects;

        while (curGameObject != lastGameObject)
        {
            CGameObject gameObject;
            um::ReadMemory(curBaseObject.object, &gameObject);

            char buf[255];
            um::ReadMemory(gameObject.m_ObjectName, &buf);

            if (!strcmp(buf, m_Name))
            {
                return curGameObject;
            }

            um::ReadMemory(curBaseObject.nextObjectLink, &curBaseObject);
            curGameObject = curBaseObject.object;
        }

    }
    static CGameObject* FindGameObjectOfType(const char* m_TypeName)
    {
        // not implement yet...
    }
    static CGameObject* TaggedObjectIterator(std::function<CGameObject* (void*, CGameObject)> loop_func)
    {
        RefreshObject();
        LastObjectBase lastTaggedObject;
        um::ReadMemory(GOM.lastTaggedObject, &lastTaggedObject);

        BaseObject TaggedObjects;
        um::ReadMemory(GOM.taggedObjects, &TaggedObjects);

        if ((uintptr_t)GOM.taggedObjects == (uintptr_t)GOM_Ptr)
        {
            return nullptr;
        }

        CGameObject* lastGameObject = lastTaggedObject.lastObject;

        CGameObject gameObject;
        um::ReadMemory(lastGameObject, &gameObject);

        if (loop_func(lastGameObject, gameObject))
        {
            return lastGameObject;
        }

        CGameObject* curGameObject = TaggedObjects.object;

        BaseObject curBaseObject = TaggedObjects;

        while (curGameObject != lastGameObject)
        {
            CGameObject gameObject;
            um::ReadMemory(curBaseObject.object, &gameObject);

            if (loop_func(curGameObject, gameObject))
            {
                return curGameObject;
            }

            um::ReadMemory(curBaseObject.nextObjectLink, &curBaseObject);
            curGameObject = curBaseObject.object;
        }

        return nullptr;
    }
    static CGameObject* ActiveObjectIterator(std::function<CGameObject* (void*, CGameObject)> loop_func)
    {
        RefreshObject();
        LastObjectBase activeObject;
        um::ReadMemory(GOM.lastActiveObject, &activeObject);

        BaseObject activeObjects;
        um::ReadMemory(GOM.activeObjects, &activeObjects);

        if ((uintptr_t)GOM.activeObjects == (uintptr_t)GOM_Ptr)
        {
            return nullptr;
        }

        CGameObject* lastGameObject = activeObject.lastObject;

        CGameObject gameObject;
        um::ReadMemory(lastGameObject, &gameObject);

        if (loop_func(lastGameObject, gameObject))
        {
            return lastGameObject;
        }

        CGameObject* curGameObject = activeObjects.object;

        BaseObject curBaseObject = activeObjects;

        while (curGameObject != lastGameObject)
        {
            CGameObject gameObject;
            um::ReadMemory(curBaseObject.object, &gameObject);

            if (loop_func(curGameObject, gameObject))
            {
                return curGameObject;
            }

            um::ReadMemory(curBaseObject.nextObjectLink, &curBaseObject);
            curGameObject = curBaseObject.object;
        }

        return nullptr;
    }
    static CGameObject* UntaggedObjectIterator(std::function<CGameObject* (void*, CGameObject)> loop_func)
    {
        RefreshObject();
        LastObjectBase UntaggedObject;
        um::ReadMemory(GOM.lastUntaggedObject, &UntaggedObject);

        BaseObject UntaggedObjects;
        um::ReadMemory(GOM.UntaggedObjects, &UntaggedObjects);

        if ((uintptr_t)GOM.UntaggedObjects == (uintptr_t)GOM_Ptr)
        {
            return nullptr;
        }

        CGameObject* lastGameObject = UntaggedObject.lastObject;

        CGameObject gameObject;
        um::ReadMemory(lastGameObject, &gameObject);

        if (loop_func(lastGameObject, gameObject))
        {
            return lastGameObject;
        }

        CGameObject* curGameObject = UntaggedObjects.object;

        BaseObject curBaseObject = UntaggedObjects;

        while (curGameObject != lastGameObject)
        {
            CGameObject gameObject;
            um::ReadMemory(curBaseObject.object, &gameObject);

            if (loop_func(curGameObject, gameObject))
            {
                return curGameObject;
            }

            um::ReadMemory(curBaseObject.nextObjectLink, &curBaseObject);
            curGameObject = curBaseObject.object;
        }

        return nullptr;
    }
    static CGameObject* ObjectIterator(std::function<CGameObject* (void*, CGameObject)> loop_func, LoopType loop_type = eLOOPTYPE_All)
    {
        CGameObject* object = nullptr;
        switch (loop_type)
        {
        case eLOOPTYPE_All:
        {
            object = UntaggedObjectIterator(loop_func);
            if (object)
                return object;
            object = TaggedObjectIterator(loop_func);
            if (object)
                return object;
            object = ActiveObjectIterator(loop_func);
            if (object)
                return object;
        }
        break;
        case eLOOPTYPE_Tagged:
        {
            object = TaggedObjectIterator(loop_func);
            if (object)
                return object;
        }
        break;
        case eLOOPTYPE_UnTagged:
        {
            object = UntaggedObjectIterator(loop_func);
            if (object)
                return object;
        }
        break;
        case eLOOPTYPE_Active:
        {
            object = ActiveObjectIterator(loop_func);
            if (object)
                return object;
        }
        break;
        default:
            break;
        }
        return nullptr;
    }
    static std::vector<CGameObject*> ObjectIteratorVector(std::function<CGameObject* (void*, CGameObject)> loop_func)
    {
        RefreshObject();
        std::vector<CGameObject*> objects;

        LastObjectBase lastTaggedObject;
        um::ReadMemory(GOM.lastTaggedObject, &lastTaggedObject);

        BaseObject TaggedObjects;
        um::ReadMemory(GOM.taggedObjects, &TaggedObjects);

        if ((uintptr_t)GOM.taggedObjects == (uintptr_t)GOM_Ptr)
        {
            return objects;
        }

        CGameObject* lastGameObject = lastTaggedObject.lastObject;

        CGameObject gameObject;
        um::ReadMemory(lastGameObject, &gameObject);

        if (loop_func(lastGameObject, gameObject))
        {
            objects.push_back(lastGameObject);
        }

        CGameObject* curGameObject = TaggedObjects.object;

        BaseObject curBaseObject = TaggedObjects;

        while (curGameObject != lastGameObject)
        {
            CGameObject gameObject;
            um::ReadMemory(curBaseObject.object, &gameObject);

            if (loop_func(curGameObject, gameObject))
            {
                objects.push_back(curGameObject);
            }

            um::ReadMemory(curBaseObject.nextObjectLink, &curBaseObject);
            curGameObject = curBaseObject.object;
        }

        return objects;
    }
}