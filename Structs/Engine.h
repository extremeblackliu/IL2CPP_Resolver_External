#pragma once

struct Quaternion
{
	float x, y, z, w;

	Quaternion() { x = y = z = w = 0.f; }
	Quaternion(float f1, float f2, float f3, float f4) { x = f1; y = f2; z = f3; w = f4; }
};

struct Vector2
{
	float x, y;

	Vector2() { x = y = 0.f; }
	Vector2(float f1, float f2) { x = f1; y = f2; }
};

struct Vector3
{
	float x, y, z;

	Vector3() { x = y = z = 0.f; }
	Vector3(float f1, float f2, float f3) { x = f1; y = f2; z = f3; }
};

struct Vector4
{
	float x, y, z, w;

	Vector4() { x = y = z = w = 0.f; }
	Vector4(float f1, float f2, float f3, float f4) { x = f1; y = f2; z = f3; w = f4; }
};