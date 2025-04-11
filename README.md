## IL2CPP Resolver External
A run-time API resolver for IL2CPP Unity in External.

[Internal version](https://github.com/sneakyevilSK/IL2CPP_Resolver)

# ARCHIVED
External version is impossible, i started this because i need research for my own project.

Soon i realize, some code signature i made cross only few unity versions.

that means if i have to make all unity available, billion signature must be recreate, impossible for me.

code is shit tho dont even try it

thanks everyone

# Initialize
```cpp
#include "IL2CPP_Resolver_External/IL2CPP_Resolver.h"

void main()
{
    IL2CPP::Initialize();
}
```

# Quick Examples
N/A

# WARNING
* This is still working-in-progress.

# Requirements
* C++ 17 or Greater.
* Windows only.

# TODO
* Auto-Detect System Class and Game Class for component looping
* More Unity Version Support
* More Methods
* Fetch Method from Module by giving Method's Name

### Tested on
* VRChat[<= Unity 2019.4.31]
* VRChat[Unity 2022.3.6] (testing...)
