#pragma once

struct GameObject : Object
{
	IL2CPP_PAD(0x20);			// 0x28
	void* m_ComponentList;		// 0x30
	IL2CPP_PAD(0x8);
	int m_iComponents;			// 0x40
	IL2CPP_PAD(0x16);
	bool m_active;				// 0x56
	bool m_activeCached;		// 0x57
	IL2CPP_PAD(0x2);
	char* m_ObjectName;			// 0x60
};