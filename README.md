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

### WARNING
* This is still working-in-progress.
* Basic struct maybe DIFFERENT between DIFFERENT Unity Engine version.
* The code for this project is still messy, since I spent most time on reversing Unity's functions in IDA instead coding it, which I'll sort into neat and readable code later.

### Requirements
* C++ 17 or greater

### TODO
* More Unity Version Support
* More Methods

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
