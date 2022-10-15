#pragma once
#include <cstdint>

struct CharacterController
{
	char m_pad0[0x38];
	void* unk_0; //0x38
	char m_pad1[0x8];
	void* unk_1; //0x48
	char m_pad2[0x1D];
	bool enable_Collider; //0x6D
	bool enable_Collider_dup; //0x6C
	char m_pad3[0x3];
	void* unk_2; //0x70
};

struct Rigidbody
{
	char m_pad0[0x98];
	bool isKinematic;	//0x98
	char m_pad1[0x11];
	bool isKinematic_dup; //	
};