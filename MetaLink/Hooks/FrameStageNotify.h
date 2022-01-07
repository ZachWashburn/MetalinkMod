#include "../BaseIncludes.h"
#include "SDK.h"



typedef void(__thiscall* FrameStageNotifyFunc_t)(CHLClient*, ClientFrameStage_t);
void __fastcall hk_CHLClient_FrameStageNotify(CHLClient*, void*, ClientFrameStage_t);