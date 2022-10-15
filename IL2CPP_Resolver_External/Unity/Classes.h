#pragma once
#include <cstdint>

struct Object
{
	void* m_Class;				// 0x0
	int m_pad0;					// 0x4
	int m_InstanceID;			// 0x8
};

struct GameObject : Object
{
	char m_pad1[0x20];			// 0x28
	void* m_ComponentList;		// 0x30
	char m_pad2[0x8];			
	int m_iComponents;			// 0x40
	char m_pad3[0x16];			
	bool m_active;				// 0x56
	bool m_activeCached;		// 0x57
	char m_pad4[0x2];			
	char* m_ObjectName;			// 0x60
};

struct Vector3
{
	float x, y, z;
};

struct Transform
{
	char m_pad0[0x90];		// 0x00
	Vector3 m_Position;		// 0x90
};

struct Component : Object
{
	char m_pad0[0x18];			// 0x27
	Component* m_Component;		// 0x28
	char m_pad1[0x8];			//
	Transform* m_Transform;		// 0x38
};

struct ComponentList
{
	Component* m_Component;
	void* unk;
};

struct BaseObject
{
	char pad_0x0000[0x8];		//0x0000
	BaseObject* nextObjectLink; //0x0008 
	GameObject* object;			//0x0010 
};

struct LastObjectBase
{
	char pad0[0x10];		//0x0000
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