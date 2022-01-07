#pragma once
#include "../BaseIncludes.h"
#include "mathlib/mathlib.h"
#include "VirtualFunction.h"
class CUserCmd;
class IInput {
public:
    std::byte pad[12];
    bool isTrackIRAvailable;
    bool isMouseInitialized;
    bool isMouseActive;
    std::byte pad1[158];
    bool isCameraInThirdPerson;
    std::byte pad2;
    Vector cameraOffset;

    CUserCmd* GetUserCmd(int nSlot, int sequence_number)
    {
        return CallVirtualFunction<CUserCmd*,  int, int>(this,8, nSlot, sequence_number);
    }

};



class ToggleKey
{
public:
    
private:
    int m_nKey = 0;
    float m_flLastPressTime = 0.f;
    float m_flWaitTime = 0.f;
};
