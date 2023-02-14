#include <iostream>
#include "Unity/Unity.h"


int main()
{
	if (!um::Init(FindWindowA(nullptr, "Game")))
	{
	   return 0;
	}

	if (!Unity::Initialize())
	{
	    return 0;
	}
	
	while(true)
	{
		// do something
	}
}
