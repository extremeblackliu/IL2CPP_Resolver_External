#pragma once

#include <functional>
#include <vector>

#include "Utils/Memory.hpp"

// All Unity Classes/Structs
#include "Classes/Classes.h"

// Namespaces
#include "Base/GameObject.hpp"


// namespace IL2CPP
#include "IL2CPP.h"

enum LoopType
{
    eLOOPTYPE_All,
    eLOOPTYPE_Tagged,
    eLOOPTYPE_UnTagged,
    eLOOPTYPE_Active
};

namespace IL2CPP
{
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

        GameObject::GOM_Ptr = (GameObjectManager*)GOM;
        um::ReadMemory(GameObject::GOM_Ptr, &GOM);

        // Initialize IL2CPP Classes from il2cpp_domain_get_assemblies
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

        IL2CPPClass::AssemblyCount = ((uintptr_t)End - (uintptr_t)Start) / sizeof(void*);

        IL2CPPClass::AssembilesTable = (void**)malloc(IL2CPPClass::AssemblyCount * sizeof(void*));

        um::ReadMemory(Start, IL2CPPClass::AssembilesTable, IL2CPPClass::AssemblyCount * sizeof(void*));
        
        // xref 48 8B 05 ? ? ? ? 48 83 3C 03 00 75 45 48 8D 35 ? ? ? ? 48 89 74 24 38 48 8B CE (GameAssembly.dll)
        // mov rax,[7FFE238F2278]
        addr = um::SignatureScan(L"GameAssembly.dll", "48 8B 05 ? ? ? ? 48 83 3C 03 00 75 45 48 8D 35 ? ? ? ? 48 89 74 24 38 48 8B CE");
        addr = helper::relativeToAbsolute(addr,3);

        IL2CPPClass::ClassesTable = (void*)(addr);
        um::ReadMemory(IL2CPPClass::ClassesTable, &IL2CPPClass::ClassesTable);
        if (!IL2CPPClass::ClassesTable)
        {
            return false;
        }

        return true;
    }
}
