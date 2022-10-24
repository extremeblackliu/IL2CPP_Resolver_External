#pragma once

struct Transform
{
	IL2CPP_PAD(0x90);		// 0x00
	Vector3 m_Position;		// 0x90
	IL2CPP_PAD(0x8);
	Vector3 m_Rotate;		// 0xA4 (express in -1.f to 1.f)
};