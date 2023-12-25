#pragma once

struct CTransform
{
	IL2CPP_PAD(0x90);		// 0x00
	Vector3 m_Position;		// 0x90
	IL2CPP_PAD(0x8);
	Vector3 m_Rotate;		// 0xA4 (express in -1.f to 1.f)

    Vector3 GetPosition()
    {
        CTransform _Transform;
        um::ReadMemory(this, &_Transform);

        return _Transform.m_Position;
    }

    void SetPosition(Vector3 p_Position)
    {
        um::WriteMemory(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(this) + offsetof(CTransform, m_Position)), &p_Position);
        // Note: The Game maybe has some Physics system that is always writing to y axis, you need find the way to disable it yourself.
    }

    // return in range -180.f to 180.f
    Vector3 GetRotation()
    {
        CTransform _Transform;
        um::ReadMemory(this, &_Transform);
        return Vector3(180.f * _Transform.m_Rotate.x, 180.f * _Transform.m_Rotate.y, 180.f * _Transform.m_Rotate.z);
    }

    void SetRotation(Vector3 p_Rotate)
    {
        um::WriteMemory(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(this) + offsetof(CTransform, m_Rotate)), &p_Rotate);
    }
};