#pragma once

#include <cmath>

constexpr float PI = 3.14159265358979323846f;

#define sqrt(x)		std::sqrtf(x)
#define Sin(x)		std::sinf(x)
#define Cos(x)		std::cosf(x)
#define Fabs(x)     std::fabsf(x)
#define FMod(x, Y)  std::fmodf((x), (y))
#define Acos(x)     std::acosf(x)
#define Atan2(y, x) std::atan2f((y), (x))
#define Ceil(x)     std::ceilf(x)
#define Pow(x, y)   std::powf(x, y)

#define Tan(item) (Sin(item) / Cos(item))

#define Min(item1, item2) ((item1 < item2) ? item1 : item2)
#define Max(item1, item2) ((item1 > item2) ? item1 : item2)

#define Abs(item) ((item > 0) ? item : item * -1)

#define Floor(item) (static_cast<float>(static_cast<int>(item)))
#define Round(item) (static_cast<float>(static_cast<int>(item + 0.5f)))

__forceinline constexpr float RAD2DEG(float val) { return (val * (180.f / PI)); }
__forceinline constexpr float DEG2RAD(float val) { return (val * (PI / 180.f)); }

class Vector;
class QAngle;
class Matrix3x4;

#if 0
void AngleVectors(const QAngle& angles, Vector* forward, Vector* right = nullptr, Vector* up = nullptr);

void VectorAngles(const Vector& forward, QAngle* angles);

float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle);

QAngle CalcAngle(const Vector& src, const Vector& dst);

static float AngleNormalize(float angle)
{
	if (angle >= -180.f && angle <= 180.f)
		return angle;

	const float rotation = Round(Abs(angle / 360.f));

	return (angle < 0.f) ? angle + (360.f * rotation) : angle - (360.f * rotation);
}

float AngleMod(float angle) { return ((360.f / 65536.f) * (int(angle * (65536.f / 360.f)) & 65535)); }

static float ApproachAngle(float tar, float val, float sp)
{
	const float speed = Abs(sp);
	const float target = AngleMod(tar);

	float value = AngleMod(val);
	float delta = AngleNormalize(target - value);

	delta > speed ? value += speed : (delta < -speed ? value -= speed : value = target);

	return value;
}

void AngleNormalize(float* angle) { *angle = AngleNormalize(*angle); }

template <class T>
static void VSwap(T& x, T& y)
{
	T temp = x;
	x = y;
	y = temp;
}

template<typename T>
static T Clamp(const T tItem, const T tMin, const T tMax)
{
	T tTemp = tItem;

	if (tItem > tMax)
		tTemp = tMax;
	else if (tItem < tMin)
		tTemp = tMin;

	return tTemp;
}

template<typename T>
void Clamp(T* tItem, const T tMin, const T tMax) { *tItem = Clamp(*tItem, tMin, tMax); }
#endif