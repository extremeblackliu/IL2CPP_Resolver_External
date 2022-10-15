## IL2CPP Resolver External
A run-time API resolver for IL2CPP Unity in External way.

Internal version:[https://github.com/sneakyevilSK/IL2CPP_Resolver]

### WARNING
this is still working-in-progress.
some struct maybe different between different unity engine version.

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
Unity::ObjectIterator(
    [](void* ptr, GameObject object) -> GameObject* {
        return nullptr;
}, eLOOPTYPE_UnTagged // eLOOPTYPE_Active | eLOOPTYPE_Tagged | eLOOPTYPE_UnTagged
);
```
