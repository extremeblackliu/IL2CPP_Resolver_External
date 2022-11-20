## IL2CPP Resolver External
A run-time API resolver for IL2CPP Unity in External way.

Internal version:[https://github.com/sneakyevilSK/IL2CPP_Resolver]

### Note
i have provided basic memory r/w and helper functions in Unity/rw/usermode.h.

if you are using for kernel project, you need replace with your own one.

**include to your project with ./Unity folder and**
```cpp
#include "./Unity/Unity.h" 
```
Initialize
```cpp
if (!um::Init(FindWindowA(nullptr, "Game")))
{
   return 0;
}

if (!Unity::Initialize())
{
    return 0;
}
```
if you have more unity different version struct or code improve, pull request!

### WARNING
* This is still working-in-progress.
* Basic struct maybe DIFFERENT between DIFFERENT Unity Engine version.(Change in [Classes.h](https://github.com/extremeblackliu/IL2CPP_Resolver_External/blob/master/IL2CPP_Resolver_External/Unity/Classes/Classes.h))

### Requirements
* C++ 17 or greater

### TODO
* Auto-Detect System Class and Game Class for component looping
* More Unity Version Support
* More Methods
* Fetch Method from Module by giving Method's Name

### Tested on
* VRChat[Unity 2019.4]

### Quick Example

```cpp
GameObject* GetLocalPlayer()
{
    GameObject* Player = Unity::ObjectIterator(
        [](void* ptr, GameObject object) -> GameObject* {

            char buf[255];
            um::ReadMemory(object.m_ObjectName, &buf);
            if (buf[10] == 'L') // L - Local , R - Remote
            {
                return (GameObject*)ptr;
            }
            return nullptr;
        }
    );
    return Player;
}

void Hack()
{
    // example manipulate of rigidbody.isKinematic
    GameObject* Player = GetLocalPlayer();
    
    Component* Component = Unity::GetComponentByIndex(Player, 1, true);
    if(!Component)
       return;
    
    Rigidbody rigidbody;
    um::ReadMemory(Component, &rigidbody);
    
    rigidbody.isKinematic = false;
    
    um::WriteMemory(Component, &rigidbody);
}
```

Position Get/Set:
```cpp
// Teleport to self
void Teleport()
{
    GameObject* Player = GetLocalPlayer();
    
    Vector3 position = Unity::GetPosition(Player);
   
    Unity::SetPosition(Player, position); 
}

```

loop through different type(Active,Tagged,UnTagged) of GameObjects in ALL Scenes:
```cpp
// ObjectIterator lambda: return nullptr to make continue loop.
Unity::ObjectIterator(
    [](void* ptr, GameObject object) -> GameObject* {
        return nullptr;
}, eLOOPTYPE_UnTagged // eLOOPTYPE_Active | eLOOPTYPE_Tagged | eLOOPTYPE_UnTagged
);
```

find the class we want, and use it for find the component
```cpp
// Example of VRChat
namespace GlobalVar // put it on some globalvar, cache it
{
    il2cppClass* VRCPickup;
}

void CacheClasses()
{
    std::vector<std::pair<il2cppClass*, il2cppClass>> Classes;
    IL2CPP::FetchClasses(&Classes, "VRCSDK3");  // in other game, game classes are mostly in Assembly-CSharp.dll

    GlobalVar::VRCPickup = IL2CPP::FilterClass(Classes, "VRCPickup", "VRC.SDK3.Components");
}

void Hack()
{
    Unity::ObjectIterator(
            [](void* ptr, GameObject object) -> GameObject* {
               
            // the loop is manually, see TODO #1
                    
            void* buffer = malloc(object.m_iComponents * sizeof(ComponentList));
            um::ReadMemory((void*)((uintptr_t)object.m_ComponentList + 8), buffer, object.m_iComponents * sizeof(ComponentList));
            if (!buffer)
            {
                return nullptr;
            }

            for (int i = 0; i < object.m_iComponents; i++)
            {
                ComponentList* list = (ComponentList*)buffer;

                Component component;
                um::ReadMemory(list[i].m_Component, &component);

                il2cppClass* klass = IL2CPP::GetClassByComponent(component.m_Component, false);

                if (klass == Cache::VRCPickup) // we compare the il2cppClass Pointer, it will be same
                {
                    bool DisallowTheft = false;
                    um::WriteMemory((void*)((uintptr_t)component.m_Component + 0x1C), &DisallowTheft); // use component.m_Component because its game class
                    // or do something to component...
                    break;
                }
            }
            free(buffer);


            return nullptr;
        }, eLOOPTYPE_UnTagged
    );
}
```
