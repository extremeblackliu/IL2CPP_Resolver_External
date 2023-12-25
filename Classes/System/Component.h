#pragma once

struct CComponent : CObject
{
	IL2CPP_PAD(0x18);			// 0x27
	CComponent* m_Component;		// 0x28
	IL2CPP_PAD(0x8);			//
	CTransform* m_Transform;		// 0x38
};