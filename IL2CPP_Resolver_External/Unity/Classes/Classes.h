#pragma once

#define IL2CPP_PAD_INSERT(x, y) x ## y
#define IL2CPP_PAD_DEFINE(x, y) IL2CPP_PAD_INSERT(x, y)
#define IL2CPP_PAD(size) char IL2CPP_PAD_DEFINE(padding_, __LINE__)[size]

// include IL2CPP Classes
#include "IL2CPP/IL2CPPClasses.h"

// include Structs
#include "../Structs/Engine.h"
#include "../Structs/System_String.h"

// inlcude System Classes
#include "System/Object.h"
#include "System/GameObject.h"
#include "System/Transform.h"
#include "System/Component.h"
#include "System/String.h"

#include "System/CharacterController.h"
#include "System/Rigidbody.h"

struct ComponentList
{
	Component* m_Component;
	void* unk;
};

struct BaseObject
{
	IL2CPP_PAD(0x8);
	BaseObject* nextObjectLink; //0x0008 
	GameObject* object;			//0x0010 
};

struct LastObjectBase
{
	IL2CPP_PAD(0x10);
	GameObject* lastObject; //0x0010 
};

struct GameObjectManager {
	LastObjectBase* lastTaggedObject; //0x0000 
	BaseObject* taggedObjects; //0x0008
	LastObjectBase* lastActiveObject; //0x0010 
	BaseObject* activeObjects; //0x0018
	LastObjectBase* lastUntaggedObject;
	BaseObject* UntaggedObjects;
};