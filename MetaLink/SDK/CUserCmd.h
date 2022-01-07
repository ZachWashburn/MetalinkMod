#pragma once

#include "mathlib/vector.h"
#include "../../ThirdParty/Hash.h"
#include "tier1/bitbuf.h"

#define IN_ATTACK    (1 << 0)
#define IN_JUMP      (1 << 1)
#define IN_DUCK      (1 << 2)
#define IN_FORWARD   (1 << 3)
#define IN_BACK      (1 << 4)
#define IN_USE       (1 << 5)
#define IN_CANCEL    (1 << 6)
#define IN_LEFT      (1 << 7)
#define IN_RIGHT     (1 << 8)
#define IN_MOVELEFT  (1 << 9)
#define IN_MOVERIGHT (1 << 10)
#define IN_ATTACK2   (1 << 11)
#define IN_RUN       (1 << 12)
#define IN_RELOAD    (1 << 13)
#define IN_ALT1      (1 << 14)
#define IN_ALT2      (1 << 15)
#define IN_SCORE     (1 << 16)
#define IN_SPEED     (1 << 17)
#define IN_WALK      (1 << 18)
#define IN_ZOOM      (1 << 19)
#define IN_WEAPON1   (1 << 20)
#define IN_WEAPON2   (1 << 21)
#define IN_BULLRUSH  (1 << 22)
#define IN_GRENADE1  (1 << 23)
#define IN_GRENADE2  (1 << 24)
#define IN_LOOKSPIN  (1 << 25)

class CUserCmd
{
public:
    virtual ~CUserCmd() { };
    unsigned long GetChecksum() const
    {
        unsigned long crc;

        CRC32::Init(&crc);
        CRC32::ProcessBuffer(&crc, &m_nCmdNumber, sizeof(m_nCmdNumber));
        CRC32::ProcessBuffer(&crc, &m_nTickCount, sizeof(m_nTickCount));
        CRC32::ProcessBuffer(&crc, &m_qaViewAngles, sizeof(m_qaViewAngles));
        CRC32::ProcessBuffer(&crc, &m_vAimDirection, sizeof(m_vAimDirection));
        CRC32::ProcessBuffer(&crc, &m_flForwardMove, sizeof(m_flForwardMove));
        CRC32::ProcessBuffer(&crc, &m_flSideMove, sizeof(m_flSideMove));
        CRC32::ProcessBuffer(&crc, &m_flUpMove, sizeof(m_flUpMove));
        CRC32::ProcessBuffer(&crc, &m_nButtons, sizeof(m_nButtons));
        CRC32::ProcessBuffer(&crc, &m_cImpulse, sizeof(m_cImpulse));
        CRC32::ProcessBuffer(&crc, &m_nWeaponSelect, sizeof(m_nWeaponSelect));
        CRC32::ProcessBuffer(&crc, &m_nWeaponSubtype, sizeof(m_nWeaponSubtype));
        CRC32::ProcessBuffer(&crc, &m_nRandomSeed, sizeof(m_nRandomSeed));
        CRC32::ProcessBuffer(&crc, &m_nsMousedx, sizeof(m_nsMousedx));
        CRC32::ProcessBuffer(&crc, &m_nsMousedy, sizeof(m_nsMousedy));
        CRC32::Final(&crc);

        return crc;
    }

    int m_nCmdNumber;
    int m_nTickCount;
    QAngle m_qaViewAngles;
    Vector m_vAimDirection;
    float m_flForwardMove;
    float m_flSideMove;
    float m_flUpMove;
    int m_nButtons;
    char m_cImpulse;
    int m_nWeaponSelect;
    int m_nWeaponSubtype;
    int m_nRandomSeed;
    short m_nsMousedx;
    short m_nsMousedy;
    bool m_bHasbeenpredicted;
private:
    QAngle m_qaHeadangles;
    Vector m_vHeadoffset;
};

class VerifiedUserCmd
{
public:
    CUserCmd cmd;
    unsigned long crc;
};

//// undef this when we get bitbuf
#if 1

static bool WriteUserCmdDeltaInt(bf_write* buf, int from, int to, int bits = 32)
{
    if (from != to)
    {
        buf->WriteOneBit(1);
        buf->WriteUBitLong(to, bits);
        return true;
    }

    buf->WriteOneBit(0);
    return false;
}

static bool WriteUserCmdDeltaShort(bf_write* buf, int from, int to)
{
    if (from != to)
    {
        buf->WriteOneBit(1);
        buf->WriteShort(to);
        return true;
    }

    buf->WriteOneBit(0);
    return false;
}

static bool WriteUserCmdDeltaFloat(bf_write* buf, float from, float to)
{
    if (from != to)
    {
        buf->WriteOneBit(1);
        buf->WriteFloat(to);
        return true;
    }

    buf->WriteOneBit(0);
    return false;
}

static bool WriteUserCmdDeltaAngle(bf_write* buf, float from, float to, int bits)
{
    if (from != to)
    {
        buf->WriteOneBit(1);
        buf->WriteBitAngle(to, bits);
        return true;
    }

    buf->WriteOneBit(0);
    return false;
}

static bool WriteUserCmdDeltaVec3Coord(bf_write* buf, const Vector& from, const Vector& to)
{
    if (from != to)
    {
        buf->WriteOneBit(1);
        buf->WriteBitVec3Coord(to);
        return true;
    }

    buf->WriteOneBit(0);
    return false;
}


static void __WriteUserCmd(bf_write* buf, const CUserCmd* to, const CUserCmd* from)
{
    WriteUserCmdDeltaInt(buf, from->m_nCmdNumber + 1, to->m_nCmdNumber, 32);
    WriteUserCmdDeltaInt(buf, from->m_nTickCount + 1, to->m_nTickCount, 32);

    WriteUserCmdDeltaFloat(buf, from->m_qaViewAngles[0], to->m_qaViewAngles[0]);
    WriteUserCmdDeltaFloat(buf, from->m_qaViewAngles[1], to->m_qaViewAngles[1]);
    WriteUserCmdDeltaFloat(buf, from->m_qaViewAngles[2], to->m_qaViewAngles[2]);

    WriteUserCmdDeltaFloat(buf, from->m_vAimDirection[0], to->m_vAimDirection[0]);
    WriteUserCmdDeltaFloat(buf, from->m_vAimDirection[1], to->m_vAimDirection[1]);
    WriteUserCmdDeltaFloat(buf, from->m_vAimDirection[2], to->m_vAimDirection[2]);

    WriteUserCmdDeltaFloat(buf, from->m_flForwardMove, to->m_flForwardMove);
    WriteUserCmdDeltaFloat(buf, from->m_flSideMove, to->m_flSideMove);
    WriteUserCmdDeltaFloat(buf, from->m_flUpMove, to->m_flUpMove);
    WriteUserCmdDeltaInt(buf, from->m_nButtons, to->m_nButtons, 32);
    WriteUserCmdDeltaInt(buf, from->m_cImpulse, to->m_cImpulse, 8);

    if (WriteUserCmdDeltaInt(buf, from->m_nWeaponSelect, to->m_nWeaponSelect, 11))
    {
        WriteUserCmdDeltaInt(buf, from->m_nWeaponSubtype, to->m_nWeaponSubtype, 6);
    }

    WriteUserCmdDeltaShort(buf, from->m_nsMousedx, to->m_nsMousedx);
    WriteUserCmdDeltaShort(buf, from->m_nsMousedy, to->m_nsMousedy);
}

#endif // 0
