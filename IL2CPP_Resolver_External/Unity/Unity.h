#pragma once

#include <functional>
#include <vector>

#include "Classes.h"
#include "GameClasses.h"
#include "IL2CPPClasses.h"

#include "rw/usermode.h"

namespace IL2CPP
{
    inline void** AssembilesTable;
    inline size_t AssemblyCount;
    inline void* ClassesTable;
    inline void* ClassNamePool;
    inline void* ClassNameSpacePool;

    static void FetchClasses(std::vector<std::pair<il2cppClass*, il2cppClass>>* out, const char* ModuleName)
    {
        out->clear();
        
        il2cppImage Image = {};
        for (int i = 0; i < AssemblyCount; i++)
        {
            il2cppAssembly assembly;
            um::ReadMemory((void*)AssembilesTable[i], &assembly);

            il2cppImage image;
            um::ReadMemory(assembly.m_pImage, &image);

            char Name[255];
            um::ReadMemory((void*)image.m_pNameNoExt, &Name);

            if (!assembly.m_pImage || strcmp(Name, ModuleName))
            {
                continue;
            }

            //Image = assembly.m_pImage;
            um::ReadMemory(assembly.m_pImage, &Image);

            break;
        }
        ClassNamePool = VirtualAlloc(0x0, Image.m_iClassCount * 255, MEM_COMMIT, PAGE_READWRITE);
        ClassNameSpacePool = VirtualAlloc(0x0, Image.m_iClassCount * 255, MEM_COMMIT, PAGE_READWRITE);
        for (size_t i = 0; i < Image.m_iClassCount; i++)
        {
                DWORD offset = Image.m_iClassStartOffset + i;
                if (offset == -1)
                    continue;

                offset *= 8;

            il2cppClass* Class;
            um::ReadMemory((void*)((uintptr_t)ClassesTable + offset), &Class);

            if (!Class)
            {
                continue;
            }

            il2cppClass current;
            um::ReadMemory(Class, &current);

            char* buf = (char*)((uintptr_t)ClassNamePool + i * 255);
            um::ReadMemory((void*)current.m_pName, buf, 255);
            current.m_pName = buf;

            char* buf1 = (char*)((uintptr_t)ClassNameSpacePool + i * 255);
            um::ReadMemory((void*)current.m_pNamespace, buf1, 255);
            current.m_pNamespace = buf1;
            
            out->push_back(std::make_pair(Class, current));
        }

    }

    static il2cppClass* FilterClass(std::vector<std::pair<il2cppClass*, il2cppClass>>& m_pClasses, const char* m_ClassName, const char* m_ClassNameSpace = nullptr)
    {
        for (auto& i : m_pClasses)
        {
            if (strcmp(i.second.m_pName, m_ClassName))
            {
                continue;
            }
            if (!m_ClassNameSpace)
            {
                return i.first;
            }
            else if(!strcmp(i.second.m_pNamespace, m_ClassNameSpace))
            {
                return i.first;
            }
        }
        return nullptr;
    }

    static il2cppClass* GetClassByComponent(Component* component,bool is_system_class)
    {
        il2cppClass* m_class = nullptr;
        if (is_system_class)
        {
            Component comp;
            um::ReadMemory(component, &comp);
            um::ReadMemory(comp.m_Component, &m_class);
            return m_class;
        }
        um::ReadMemory(component, &m_class);
        return m_class;
    }
}

enum LoopType
{
    eLOOPTYPE_All,
    eLOOPTYPE_Tagged,
    eLOOPTYPE_UnTagged,
    eLOOPTYPE_Active
};

namespace Unity
{
    inline GameObjectManager GOM;
    inline GameObjectManager* GOM_Ptr;

    static inline void* GameAssembly;
    static inline void* UnityPlayer;

    static bool Initialize()
    {
        GameAssembly = um::GetModuleBase(L"GameAssembly.dll");
        UnityPlayer = um::GetModuleBase(L"UnityPlayer.dll");

        // Initalize Unity GameObjectManager
        // xref E8 ? ? ? ? 48 8B D0 83 FB 05 75 (UnityPlayer.dll)
        uintptr_t addr = um::SignatureScan(L"UnityPlayer.dll", "E8 ? ? ? ? 48 8B D0 83 FB 05 75");
        addr = helper::relativeToAbsolute(addr);
        addr = helper::relativeToAbsolute(addr, 3);

        void* GOM = (void*)(addr);
        um::ReadMemory(GOM, &GOM);

        if (!GOM)
        {
            return false;
        }

        GOM_Ptr = (GameObjectManager*)GOM;
        um::ReadMemory(GOM_Ptr, &GOM);

        // Initialize IL2CPP Classes
        // xref E8 ? ? ? ? 48 8B 48 08 48 2B 08 48 C1 F9 03 48 89 0B 48 8B 00 48 83 C4 20 5B C3 (GameAssembly.dll)
        addr = um::SignatureScan(L"GameAssembly.dll", "E8 ? ? ? ? 48 8B 48 08 48 2B 08 48 C1 F9 03 48 89 0B 48 8B 00 48 83 C4 20 5B C3");
        addr = helper::relativeToAbsolute(addr);
        addr = helper::relativeToAbsolute(addr, 3);

        void* Assemblies = (void*)(addr);

        void* End = nullptr;
        void* Start = nullptr;

        um::ReadMemory(Assemblies, &Start);
        um::ReadMemory((void*)((uintptr_t)Assemblies + 0x8), &End);

        if (!Start || !End)
        {
            return false;
        }

        IL2CPP::AssemblyCount = ((uintptr_t)End - (uintptr_t)Start) / sizeof(void*);

        IL2CPP::AssembilesTable = (void**)malloc(IL2CPP::AssemblyCount * sizeof(void*));

        um::ReadMemory(Start, IL2CPP::AssembilesTable, IL2CPP::AssemblyCount * sizeof(void*));
        
        // xref 48 8B 05 ? ? ? ? 48 83 3C 03 00 75 45 48 8D 35 ? ? ? ? 48 89 74 24 38 48 8B CE (GameAssembly.dll)
        // mov rax,[7FFE238F2278]
        addr = um::SignatureScan(L"GameAssembly.dll", "48 8B 05 ? ? ? ? 48 83 3C 03 00 75 45 48 8D 35 ? ? ? ? 48 89 74 24 38 48 8B CE");
        addr = helper::relativeToAbsolute(addr,3);

        IL2CPP::ClassesTable = (void*)(addr);
        um::ReadMemory(IL2CPP::ClassesTable, &IL2CPP::ClassesTable);
        if (!IL2CPP::ClassesTable)
        {
            return false;
        }

        return true;
    }

    static void RefreshObject()
    {
        um::ReadMemory(GOM_Ptr, &GOM);
    }

    static GameObject* Find(const char* m_Name)
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

        GameObject* lastGameObject = lastTaggedObject.lastObject;

        GameObject gameObject;
        um::ReadMemory(lastGameObject, &gameObject);

        char buf0[255];
        um::ReadMemory(gameObject.m_ObjectName, &buf0);
        if (!strcmp(buf0, m_Name))
        {
            return lastGameObject;
        }

        GameObject* curGameObject = TaggedObjects.object;

        BaseObject curBaseObject = TaggedObjects;

        while (curGameObject != lastGameObject)
        {
            GameObject gameObject;
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
    static GameObject* FindGameObjectOfType(const char* m_TypeName)
    {
        // not implement yet...
    }
    static GameObject* TaggedObjectIterator(std::function<GameObject*(void*,GameObject)> loop_func)
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

        GameObject* lastGameObject = lastTaggedObject.lastObject;

        GameObject gameObject;
        um::ReadMemory(lastGameObject, &gameObject);

        if (loop_func(lastGameObject, gameObject))
        {
            return lastGameObject;
        }

        GameObject* curGameObject = TaggedObjects.object;

        BaseObject curBaseObject = TaggedObjects;

        while (curGameObject != lastGameObject)
        {
            GameObject gameObject;
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
    static GameObject* ActiveObjectIterator(std::function<GameObject* (void*, GameObject)> loop_func)
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

        GameObject* lastGameObject = activeObject.lastObject;

        GameObject gameObject;
        um::ReadMemory(lastGameObject, &gameObject);

        if (loop_func(lastGameObject, gameObject))
        {
            return lastGameObject;
        }

        GameObject* curGameObject = activeObjects.object;

        BaseObject curBaseObject = activeObjects;

        while (curGameObject != lastGameObject)
        {
            GameObject gameObject;
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
    static GameObject* UntaggedObjectIterator(std::function<GameObject* (void*, GameObject)> loop_func)
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

        GameObject* lastGameObject = UntaggedObject.lastObject;

        GameObject gameObject;
        um::ReadMemory(lastGameObject, &gameObject);

        if (loop_func(lastGameObject, gameObject))
        {
            return lastGameObject;
        }

        GameObject* curGameObject = UntaggedObjects.object;

        BaseObject curBaseObject = UntaggedObjects;

        while (curGameObject != lastGameObject)
        {
            GameObject gameObject;
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
    static GameObject* ObjectIterator(std::function<GameObject* (void*, GameObject)> loop_func, LoopType loop_type = eLOOPTYPE_All)
    {
        GameObject* object = nullptr;
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
    static std::vector<GameObject*> ObjectIteratorVector(std::function<GameObject* (void*, GameObject)> loop_func)
    {
        RefreshObject();
        std::vector<GameObject*> objects;

        LastObjectBase lastTaggedObject;
        um::ReadMemory(GOM.lastTaggedObject, &lastTaggedObject);

        BaseObject TaggedObjects;
        um::ReadMemory(GOM.taggedObjects, &TaggedObjects);

        if ((uintptr_t)GOM.taggedObjects == (uintptr_t)GOM_Ptr)
        {
            return objects;
        }

        GameObject* lastGameObject = lastTaggedObject.lastObject;

        GameObject gameObject;
        um::ReadMemory(lastGameObject, &gameObject);

        if (loop_func(lastGameObject, gameObject))
        {
            objects.push_back(lastGameObject);
        }

        GameObject* curGameObject = TaggedObjects.object;

        BaseObject curBaseObject = TaggedObjects;

        while (curGameObject != lastGameObject)
        {
            GameObject gameObject;
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
    static Transform* GetTransform(GameObject* object)
    {
        GameObject gameObject;
        um::ReadMemory(object, &gameObject);

        void* buffer = malloc(gameObject.m_iComponents * sizeof(ComponentList));
        
        if (!buffer)
        {
            return nullptr;
        }

        um::ReadMemory((void*)((uintptr_t)gameObject.m_ComponentList + 8), buffer, gameObject.m_iComponents * sizeof(ComponentList));

        ComponentList* list = (ComponentList*)buffer;

        Component component;
        um::ReadMemory(list[0].m_Component, &component);

        free(buffer);

        return component.m_Transform;
    }
    /*
        System Class has differen Offset, make sure which component you are touching.
    */
    static Component* GetComponentByIndex(GameObject* object, int index, bool system_class = false)
    {
        GameObject gameObject;
        um::ReadMemory(object, &gameObject);

        void* buffer = malloc(gameObject.m_iComponents * sizeof(ComponentList));
        um::ReadMemory((void*)((uintptr_t)gameObject.m_ComponentList + 8), buffer, gameObject.m_iComponents * sizeof(ComponentList));

        
        ComponentList* list = (ComponentList*)buffer;
         
        Component component;
        if (system_class)
        {
            Component* com = list[index].m_Component;
            free(buffer);
            return com;
        }
        else
        {
            um::ReadMemory(list[index].m_Component, &component);
        }
        free(buffer);

        return component.m_Component;
    }
    /*
        This is not finished yet, if you use it, you need to know the Component if its Unity System Class.
    */
    static Component* ComponentIterator(GameObject* object, std::function<Component*(void*)> loop_func)
    {
        GameObject gameObject;
        um::ReadMemory(object, &gameObject);

        void* buffer = malloc(gameObject.m_iComponents * sizeof(ComponentList));
        um::ReadMemory((void*)((uintptr_t)gameObject.m_ComponentList + 8), buffer, gameObject.m_iComponents * sizeof(ComponentList));

        for (int i = 0; i < gameObject.m_iComponents; i++)
        {
            ComponentList* list = (ComponentList*)buffer;

            Component component;
            um::ReadMemory(list[i].m_Component, &component);

            if (loop_func(component.m_Component))
            {
                free(buffer);
                return component.m_Component;
            }

        }
        free(buffer);
        return nullptr;
    }
    static Vector3 GetPosition(GameObject* object)
    {
        GameObject gameObject;
        um::ReadMemory(object, &gameObject);

        Transform* trans = GetTransform(object);

        Transform transform;

        um::ReadMemory(trans, &transform);

        return transform.m_Position;
    }
    static void SetPosition(GameObject* object, Vector3 pos)
    {
        GameObject gameObject;
        um::ReadMemory(object, &gameObject);

        Transform* trans = GetTransform(object);
        um::WriteMemory((void*)((uintptr_t)trans + offsetof(Transform, m_Position)), &pos);
        
        // Note: The Game maybe has some Physics system that is always writing to y axis, you need find the way to disable it yourself.
    }
    /*
        Convert To String, Unity String
    */
    std::string System_String(void* System_String)
    {
        int size;
        um::ReadMemory((void*)((uintptr_t)System_String + 0x10), &size);

        wchar_t* buf = (wchar_t*)malloc((size + 1) * 2);

        um::ReadMemory((void*)((uintptr_t)System_String + 0x14), buf, (size + 1) * 2);

        std::wstring str(buf);
        
        free(buf);

        int ANSIStringLength = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), size + 1, 0, 0, 0, 0);
        
        char* buf1 = (char*)malloc(ANSIStringLength);

        WideCharToMultiByte(CP_UTF8, 0, str.c_str(), size + 1, buf1, ANSIStringLength, 0, 0);

        std::string nstr(buf1);

        free(buf1);

        return nstr;
    }
}
