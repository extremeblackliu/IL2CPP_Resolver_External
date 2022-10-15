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

### Tested Games
* VRChat[Unity 2019.4]

### Quick Example
```cpp
void UpdatePlayers()
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
}
```

