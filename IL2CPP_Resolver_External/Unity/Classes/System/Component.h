#pragma once

struct Component : Object
{
	IL2CPP_PAD(0x18);			// 0x27
	Component* m_Component;		// 0x28
	IL2CPP_PAD(0x8);			//
	Transform* m_Transform;		// 0x38
};