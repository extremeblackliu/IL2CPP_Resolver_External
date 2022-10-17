#pragma once

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
            else if (!strcmp(i.second.m_pNamespace, m_ClassNameSpace))
            {
                return i.first;
            }
        }
        return nullptr;
    }

    static il2cppClass* GetClassByComponent(Component* component, bool is_system_class)
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