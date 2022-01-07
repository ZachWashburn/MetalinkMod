#include "Math.h"

#include "../SDK/mathlib/mathlib.h"
#if 0
void AngleVectors(const QAngle& angles, Vector* forward, Vector* right, Vector* up)
{
    const float cp = Cos(DEG2RAD(angles.x)), sp = Sin(DEG2RAD(angles.x));
    const float cy = Cos(DEG2RAD(angles.y)),   sy = Sin(DEG2RAD(angles.y));
    const float cr = Cos(DEG2RAD(angles.z)),  sr = Sin(DEG2RAD(angles.z));

    if (forward)
    {
        forward->x = cp * cy;
        forward->y = cp * sy;
        forward->z = -sp;
    }

    if (right)
    {
        right->x = -1.f * sr * sp * cy + -1.f * cr * -sy;
        right->y = -1.f * sr * sp * sy + -1.f * cr * cy;
        right->z = -1.f * sr * cp;
    }

    if (up)
    {
        up->x = cr * sp * cy + -sr * -sy;
        up->y = cr * sp * sy + -sr * cy;
        up->z = cr * cp;
    }
}

void VectorAngles(const Vector& forward, QAngle* angles)
{
    float tmp, yaw, pitch;

    if (forward[1] == 0.f && forward[0] == 0.f)
    {
        yaw = 0.f;
        if (forward[2] > 0.f)
            pitch = 270.f;
        else
            pitch = 90.f;
    }
    else {
        yaw = (atan2(forward[1], forward[0]) * 180.f / PI);
        if (yaw < 0.f)
            yaw += 360.f;

        tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
        pitch = (atan2(-forward[2], tmp) * 180.f / PI);
        if (pitch < 0.f)
            pitch += 360.f;
    }

    angles->x = pitch;
    angles->y = yaw;
    angles->z = 0.f;
}

float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle)
{
    Vector ang, aim;

    AngleVectors(viewAngle, &aim, nullptr, nullptr);
    AngleVectors(aimAngle, &ang, nullptr, nullptr);

    float ret = RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
    if (std::isnan(ret))
        ret = 0.f;

    return ret;
}

QAngle CalcAngle(const Vector& src, const Vector& dst)
{
    QAngle ang;
    const Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
    const double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

    ang.x = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
    ang.y = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
    ang.x = 0.f;

    if (delta.x >= 0.f)
        ang.y += 180.f;

    return ang;
}
#endif