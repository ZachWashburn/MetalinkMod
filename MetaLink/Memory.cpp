#include "Memory.h"
#include "ThirdParty/nlohmann/json.hpp"


#include "../SDK/PredictionDefines.h"
#include "SDK.h"

class CBaseClientState;
using json = nlohmann::json;

#include <sstream>
#include <iomanip>

template <typename T>
static constexpr auto relativeToAbsolute(uintptr_t address) noexcept
{
    return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}

std::string hexStr(BYTE* data, int len)
{
    std::stringstream ss;
    ss << std::hex;

    for (int i(0); i < len; ++i)
        ss << std::setw(2) << std::setfill('0') << (int)data[i] << " ";

    return ss.str();
}

void* MemoryTools::FindPattern(const char* szModuleName, const char* szPattern, const char* szName) noexcept {

    static auto id = 0;
    ++id;
    HMODULE moduleHandle;
    if (moduleHandle = GetModuleHandleA(szModuleName)) {
        MODULEINFO moduleInfo;
        if (GetModuleInformation(GetCurrentProcess(), moduleHandle, &moduleInfo, sizeof(moduleInfo))) {
            auto start = static_cast<const char*>(moduleInfo.lpBaseOfDll);
            const auto end = start + moduleInfo.SizeOfImage;

            auto first = start;
            auto second = szPattern;

            while (first < end && *second) {
                if (*first == *second || *second == '?') {
                    ++first;
                    ++second;
                }
                else {
                    first = ++start;
                    second = szPattern;
                }
            }

            if (!*second) {
                if (g_bResolveRelative)
                {
                    const char* pE8Pos = strstr(szPattern, "\xE8");
                    if (pE8Pos && ((pE8Pos - szPattern) < 5))
                    {
                        start = start + 4 + *reinterpret_cast<DWORD*>(const_cast<char*>(start));
                    }
                }
                return const_cast<char*>(start);
            }
        }
    }
#ifdef _DEBUG
    VCON(XorStr("Unable To Find String %s\n"), szName);
    LOG(XorStr("FindPattern Failure!\n"));
    //if (IsDebuggerPresent())
    //    __debugbreak();
#endif
}


DWORD MemoryTools::CalculateOffset(const char* szModule, const char* szPattern, DWORD dwOffset, const char* szName)noexcept
{
    void* pAddress = FindPattern(szModule, szPattern, szName);

    if (!pAddress)
        return NULL;

    HMODULE mModuleHandle = GetModuleHandleA(szModule);
    MODULEINFO miModuleInfo;
    if (GetModuleInformation(GetCurrentProcess(), mModuleHandle, &miModuleInfo, sizeof(miModuleInfo))) {
        return (((DWORD*)pAddress + dwOffset) - miModuleInfo.lpBaseOfDll);
    }

}




void* MemoryTools::FindPattern(Modules nModule, const char* szPattern, DWORD dwOffset,  const char* szName) noexcept {
    static auto id = 0;
    ++id;
    Module* pModule = g_pModules->GetModule(nModule);
    auto start = reinterpret_cast<char*>(pModule->GetModuleBase());
    const auto end = start + pModule->GetModuleSize();

    auto first = start;
    auto second = szPattern;

    while (first < end && *second) {
        if (*first == *second || *second == '?') {
            ++first;
            ++second;
        }
        else {
            first = ++start;
            second = szPattern;
        }
    }

    if (!*second) {
        if (g_bResolveRelative)
        {
            start += (int)dwOffset;
            const char* pE8Pos = strstr(szPattern, "\xE8");
            if (pE8Pos && ((pE8Pos - szPattern) < 5))
            {
                start = start + 4 + *reinterpret_cast<DWORD*>(const_cast<char*>(start));
            }
        }
        return const_cast<char*>(start);
    }
#ifdef _DEBUG
    VCON(XorStr("Unable To Find String %s\n"), szName);
    LOG(XorStr("FindPattern Failure!\n"));
    //if(IsDebuggerPresent())
    //    __debugbreak();
#endif

    //mixerSetControlDetails();
}


void MemoryTools::HexPrint(
    const char* desc,
    const void* addr,
    const int len,
    int perLine
) {
    // Silently ignore silly per-line values.

    if (perLine < 4 || perLine > 64) perLine = 16;

    int i;
    unsigned char* buff = (unsigned char*)malloc(perLine + 1);
    const unsigned char* pc = (const unsigned char*)addr;

    // Output description if given.

    if (desc != NULL) VCON_RAW("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        VCON_RAW("  ZERO LENGTH\n");
        free(buff);
        return;
    }
    if (len < 0) {
        VCON_RAW("  NEGATIVE LENGTH: %d\n", len);
        free(buff);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of perLine means new or first line (with line offset).

        if ((i % perLine) == 0) {
            // Only print previous-line ASCII buffer for lines beyond first.

            if (i != 0) VCON_RAW("  %s\n", buff);

            // Output the offset of current line.

            VCON_RAW("  %04x ", i);
        }

        // Now the hex code for the specific character.

        VCON_RAW(" %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % perLine] = '.';
        else
            buff[i % perLine] = pc[i];
        buff[(i % perLine) + 1] = '\0';
    }

    // Pad out last line if not exactly perLine characters.

    while ((i % perLine) != 0) {
        VCON_RAW("   ");
        i++;
    }

    // And print the final ASCII buffer.

    VCON_RAW("  %s\n", buff);
    free(buff);
};


DWORD MemoryTools::CalculateOffset(Modules nModule, const char* szPattern, DWORD dwOffset, const char* szName)noexcept
{
    char* pAddress = 0;
    pAddress = (char*)FindPattern(nModule, szPattern, dwOffset, szName);

    if (!pAddress)
        return NULL;

    //pAddress += (int)dwOffset;

    Module* pModule = g_pModules->GetModule(nModule);
    return (DWORD)(pAddress - (char*)pModule->GetModuleBase());
}


/*4/30/21 -> E8 ? ? ? ? 89 45 08*/
void __declspec(naked) GetIndex(){ /* We Stream This In On Runtime*/
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;

    __asm push eax;
    __asm push ecx;
    __asm pop  ecx;
    __asm pop  eax;

    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;



    /* TODO: Make This Not NOP */
#if 0
    int nOffset;
    __asm mov nOffset, eax
    nOffset += 5;
    if (nOffset >= OFFSETMAX) {
        nOffset = nOffset - OFFSETMAX;
    }
    __asm mov eax, nOffset
#else
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
#endif

    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;

    __asm push eax;
    __asm push ecx;
    __asm pop ecx;
    __asm pop eax;

    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;
    __asm _emit 0x90;


    __asm ret;
}

Offset GetTrueOffset(Offset nOffset) {
    _asm mov eax, nOffset
    _asm call GetIndex
    _asm mov nOffset, eax

    if ((nOffset < 0) || (nOffset > OFFSETMAX))
    {
#ifndef _LIVE
        DEBUG_OUT_STR_VAR("nOffset Was Invalid, %d\n", nOffset);
#else 
        QuickCrash;
#endif
    }

    return nOffset;
}

Offset Memory::GetTrueOffset(Offset nOffset)
{
    _asm mov eax, nOffset
    _asm call GetIndex
    _asm mov nOffset, eax

    if ((nOffset < 0) || (nOffset > OFFSETMAX))
    {
#ifndef _LIVE
        DEBUG_OUT_STR_VAR("nOffset Was Invalid, %d\n", nOffset);
#else 
        QuickCrash;
#endif
    }

    return nOffset;
}


Memory::Memory() {

    memset(m_arrOffset, 0, sizeof(m_arrOffset));
        
//#if _DEBUG
#if 1
    m_arrOffset[GetTrueOffset(__PRESENT)] = new StaticOffset(__GAMEOVERLAYRENDERER, XorStr("\xFF\x15????\x8B\xF0\xEB\x1B"), 2, DEBUGSTR("Present"));
    m_arrOffset[GetTrueOffset(__RESET)] = new StaticOffset(__GAMEOVERLAYRENDERER, XorStr("\xC7\x45?????\xFF\x15????\x8B\xD8"), 9, DEBUGSTR("Reset"));
    m_arrOffset[GetTrueOffset(__listLeaves)] = new StaticOffset(__CLIENT, XorStr("\x56\x52\xFF\x50\x18"), 5, DEBUGSTR("ListLeaves"));
    m_arrOffset[GetTrueOffset(__RenderModels)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x83\xE4\xF8\x51\x8B\x45\x18"), DEBUGSTR("RenderModels"));
    m_arrOffset[GetTrueOffset(__SendTCPData)] = new StaticOffset(__ENGINE, XorStr("\xE8????\x33\xD2\x8D\x83????"), 1, DEBUGSTR("SendTcpData"));
    m_arrOffset[GetTrueOffset(__HOSTRUNFRAMEINPUT)] = new StaticOffset(__ENGINE, XorStr("\xE8????\xF3\x0F\x10\x45?\xF2\x0F\x10\x4D?"), 1, DEBUGSTR("HostRunFrameInput"));
    m_arrOffset[GetTrueOffset(__CBUFEXECUTE)] = new StaticOffset(__ENGINE, XorStr("\xE8????\x3B\x7B\x68"), 1, DEBUGSTR("CBufExecute"));
   // m_arrOffset[GetTrueOffset(__CLMOVE)] = new StaticOffset(__ENGINE, XorStr("\xE8????\xFF\x15????\xF2\x0F\x10\x05????\xDC\x25????\xDD\x5D\xF0"), 1, "CLMove");
    m_arrOffset[GetTrueOffset(__CLMOVE)] = new StaticOffset(__ENGINE, XorStr("\x55\x8B\xEC\x81\xEC????\x53\x56\x57\x8B\x3D????\x8A"), 0, DEBUGSTR("CLMove"));
    m_arrOffset[GetTrueOffset(__BONEACCESSOR)] = new StaticOffset(__CLIENT,  XorStr("\x8B\x10\x8D\x81????"), 4, DEBUGSTR("BoneAccessor"));
    m_arrOffset[GetTrueOffset(__SAVEDATA)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x6B\x45\x08\x34"), 1, DEBUGSTR("SaveData"));
    m_arrOffset[GetTrueOffset(__RESTOREDATA)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x6A\x01\xEB\x02\x6A\x02"), 1, DEBUGSTR("RestoreData"));
    m_arrOffset[GetTrueOffset(__LOCALPLAYER)] = new StaticOffset(__CLIENT, XorStr("\xA1????\x89\x45\xBC\x85\xC0"), 1, DEBUGSTR("LocalPlayer"));
    m_arrOffset[GetTrueOffset(__GETSEQUENCEACTIVITY)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x8B\xF1\x83"), 0, DEBUGSTR("GetSequenceActivity"));
    m_arrOffset[GetTrueOffset(__CPREDICTION_POSTNETWORKDATARECEIVED)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x83\xEC\x34\xA1????\x53\x56\x57\x8B\x40\x54"), 0, DEBUGSTR("CPrediction::PostNetworkDataReceived"));
    m_arrOffset[GetTrueOffset(__CBASEENTITY_POSTNETWORKDATARECEIVED)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x33\xF6\x6A\x02"), 1, DEBUGSTR("CBaseEntity::PostNetworkDataReceived"));
    m_arrOffset[GetTrueOffset(__CBASEPLAYER_POSTDATAUPDATE)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x0F\x57\xC9\x0F\x2F\x8E????"), 1, DEBUGSTR("CBaseEntity::PostDataUpdate"));
    m_arrOffset[GetTrueOffset(__CBASEPLAYER_ONDATACHANGED)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x85\xF6\x75\x62"), 1, DEBUGSTR("CBaseEntity::OnDataChanged"));
    m_arrOffset[GetTrueOffset(__CNETCHAN_PROCESSPACKET)] = new StaticOffset(__ENGINE, XorStr("\x55\x8B\xEC\x83\xE4\xC0\x81\xEC????\x53\x56\x57\x8B\x7D\x08\x8B\xD9"), 0, DEBUGSTR("CNetChan::ProcessPacket"));
    m_arrOffset[GetTrueOffset(__CCSPLAYER_ONDATACHANGED)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x8B\x75\x08\x8B\xD9\x57\x56\xE8????\x85\xF6"), 0, DEBUGSTR("CCSPlayer::OnDataChanged"));
    m_arrOffset[GetTrueOffset(__CBASEENTITY_M_P_STUDIOHDR)] = new StaticOffset(__CLIENT, XorStr("\x8B\x86????????\x89\x44\x24\x10\x85\xC0"), 2, DEBUGSTR("CBasePlayer::m_pStudioHdr"));
    m_arrOffset[GetTrueOffset(__LOCKSTUDIOHDR)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x51\x53\x8B\xD9\x56\x57\x8D\xB3"), 0, DEBUGSTR("CBasePlayer::LockStudioHdr"));
    m_arrOffset[GetTrueOffset(__SETUPVELOCITYRET)] = new StaticOffset(__CLIENT, XorStr("\x84\xC0\x75\x38\x8B\x0D????????\x8B\x01\x8B\x80"), 0, DEBUGSTR("SetupVelocityReturnAddress"));
    m_arrOffset[GetTrueOffset(__ACCUMULATELAYERSRET)] = new StaticOffset(__CLIENT, XorStr("\x84\xC0\x75\x0D\xF6\x87"), 0, DEBUGSTR("AccumulateLayersReturnAddress"));
    m_arrOffset[GetTrueOffset(__DOEXTRABONEPROCESSING)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x83\xE4\xF8\x81\xEC????\x53\x56\x8B\xF1\x57\x89\x74\x24\x1C"), 0, DEBUGSTR("C_CSPlayer::DoExtraBoneProcessing"));
    m_arrOffset[GetTrueOffset(__CONCATTRANSFORMS)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x8D\x44\x24\x6C"), 0, DEBUGSTR("ConcatTransforms"));
    m_arrOffset[GetTrueOffset(__GETMODELPTR)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x8B\x54\x24\x18\x89\x44\x24\x0C"), 1, DEBUGSTR("C_CSPlayer::GetModelPtr"));
    m_arrOffset[GetTrueOffset(__SETUPBONES)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x83\xE4\xF0\xB8\xD8"), 0, DEBUGSTR("C_BaseAnimating::SetupBones"));
    m_arrOffset[GetTrueOffset(__TELEPORTED)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x84\xC0\x74\x0A\x8B\x07"), 1, DEBUGSTR("C_BaseAnimating::Teleported"));
    m_arrOffset[GetTrueOffset(__SETUPBONES_ATTACHMENTHELPER)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x8B\x55\x08\x85\xD2\x74\x23"), 1, DEBUGSTR("C_BaseAnimating::SetupBones_AttachmentHelper"));
    m_arrOffset[GetTrueOffset(__CIKCONTEXT_INIT)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x83\xEC\x08\x8B\x45\x08\x56\x57\x8B\xF9\x8D\x8F"), 1, DEBUGSTR("CIKContext::Init"));/*\xE8????\x8D\x4F\xFC\xF7\xD9*/
    m_arrOffset[GetTrueOffset(__SHOULDSKIPANIMATIONFRAME)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x88\x44\x24\x0B"), 1, DEBUGSTR("C_BaseAnimating::ShouldSkipAnimationFrame"));
    m_arrOffset[GetTrueOffset(__CBASEANIMATINGIKCONTEXTOFFSET)] = new StaticOffset(__CLIENT, XorStr("\xFF\xD0\x8B\x8F????\x50"), 4, DEBUGSTR("C_BaseAnimating::m_pIK"));
    m_arrOffset[GetTrueOffset(__CBASEANIMATINGSTUDIOHDROFFSET)] = new StaticOffset(__CLIENT, XorStr("\x8B\xB7????\x89\x74\x24\x20?"), 2, DEBUGSTR("C_BaseAnimating::CStudioHdr"));
    m_arrOffset[GetTrueOffset(__CBASEANIMATINGEFLAGS)] = new StaticOffset(__CLIENT, XorStr("\x83\x8F?????\x8D\x47\xFC"), 2, DEBUGSTR("C_BaseAnimating::m_EFlags"));
    //m_arrOffset[GetTrueOffset(__CBASEANIMATINGENTCLIENTFLAGS)] = new StaticOffset(__CLIENT, XorStr("\x8B\x06\x83\xB8?????\x7E\x2E"), 12, "C_BaseAnimating::m_EntClientFlags");
     m_arrOffset[GetTrueOffset(__CBASEANIMATINGENTCLIENTFLAGS)] = new StaticOffset(__CLIENT, XorStr("\xF6\x47\x64\x02"), 2, DEBUGSTR("C_BaseAnimating::m_EntClientFlags"));
     m_arrOffset[GetTrueOffset(__CIKCONTEXTUPDATETARGETS)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x8B\x47\xFC\x8D\x4F\xFC\xF3\x0F\x10\x44\x24?"), 1, DEBUGSTR("CIKContext::UpdateTargets"));
    m_arrOffset[GetTrueOffset(__CIKCONTEXTCONSTRUCTOR)] = new StaticOffset(__CLIENT, XorStr("\xE8????\xA1????\xFF\x75\x18"), 1, XorStr("CIKContext::CIKContext()"));
    m_arrOffset[GetTrueOffset(__UPDATEIKLOCKS)] = new StaticOffset(__CLIENT, XorStr("\x8B\x01\xF3\x0F????\x51\xF3????\xFF\x90????\x8B\x8F????\x8D"), 0, DEBUGSTR("UpdateIKLocks"));
    m_arrOffset[GetTrueOffset(__CALCULATEIKLOCKS)] = new StaticOffset(__CLIENT, XorStr("\x8D\x4F\xFC\xF3?????\x51\xF3????\xFF\x90????\x8B\x8F????\x8D"), 0, DEBUGSTR("CalculateIKLocks"));
    m_arrOffset[GetTrueOffset(__CIKCONTEXTSOLVEDEPENDENCIES)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x8B\x44\x24\x40\x8B\x4D\x0C"), 1, DEBUGSTR("CIKContext::SolveDependencies")); // \x55\x8B\xEC\x83\xE4\xF0\x81\xEC????\x8B\x81????
    m_arrOffset[GetTrueOffset(__STANDARDBLENDINGRULES)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x83\xE4\xF0\xB8????\xE8????\x56\x8B\x75\x08\x57\x8B\xF9\x85\xF6"), 0, DEBUGSTR("StandardBlendingRules"));
    m_arrOffset[GetTrueOffset(__C_CSPLAYER_SETUPBONES)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x56\x8B\xF1\x51\x8D\x4E\xFC"), 0, DEBUGSTR("C_CSPlayer::SetupBones"));
    m_arrOffset[GetTrueOffset(__CACHEDBONEMATRIXOFFSET)] = new StaticOffset(__CLIENT, XorStr("\x50\xFF\xB7????\x52"), 3, DEBUGSTR("Cached Bone Offset"));
    m_arrOffset[GetTrueOffset(__CBASEANIMATIONGBUILDTRANSFORMATIONS)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x83\xE4\xF0\x81\xEC????\x56\x57\x8B\xF9\x8B\x0D????\x89\x7C\x24\x1C"), 0, DEBUGSTR("C_BaseAnimating::BuildTransformations"));
    m_arrOffset[GetTrueOffset(__ATTACHMENTS)] = new StaticOffset(__CLIENT, XorStr("\x8B\x92??\x00\x00\x8D\x0C\x80\x53\x8B\x1D"), 2, DEBUGSTR("C_BaseAnimating::m_Attachments"));
    m_arrOffset[GetTrueOffset(__CBASEANIMATINGSETUPBONELOCK)] = new StaticOffset(__CLIENT, XorStr("\x89\x44\x24\x0C\x3B\x02\x74\x1E"), -4, DEBUGSTR("C_BaseAnimating::m_BoneSetupLock"));
    m_arrOffset[GetTrueOffset(__CBASEANIMATINGFORMATVIEWMODELATTACHMENT)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x83\x45\xF8\x5C"), 1, DEBUGSTR("C_BaseAnimating::FormatViewModelAttachment"));
    m_arrOffset[GetTrueOffset(__GROUPSTUDIOHDR)] = new StaticOffset(__CLIENT, XorStr("\x55\x8B\xEC\x56\x8B\xF1\x57\x85\xF6"), 0, DEBUGSTR("CStudioHdr::GroupStudioHdr"));
    m_arrOffset[GetTrueOffset(__SENDSUBCHANNEL)]= new StaticOffset(__ENGINE, XorStr("\xE8????\x84\xC0\x74\x05\x80\x4C\x24??"), 1, DEBUGSTR("CNetChannel::SendSubChannelData"));
    m_arrOffset[GetTrueOffset(__NETCREATECHANNEL)] = new StaticOffset(__ENGINE, XorStr("\xFF\x05????\x8B\x44\x24\x08\x5F\x5E\x8B\xE5\x5D\xC3"), 0, DEBUGSTR("NET_CreateChannel (Mid Function)"));
    m_arrOffset[GetTrueOffset(__HOSTRUNFRAMERENDER)] = new StaticOffset(__ENGINE, XorStr("\xE8????\xE8????\x80\x3D?????\x74\x05"), 1, DEBUGSTR("HOST_RUN_FRAME_RENDER"));
    m_arrOffset[GetTrueOffset(__ICLIENTENTITYLIST)] = new StaticOffset(__CLIENT, XorStr("\xA1????\xFF\x10\xA1????\x8D\x8B"), 1, DEBUGSTR("IClientEntityList"));
    m_arrOffset[GetTrueOffset(__CSHADERSYSTEM_DRAWELEMENTS)] = new StaticOffset(__MATERIALSYSTEM, XorStr("\x55\x8B\xEC\x53\x8B\xD9\x8B\x0D????\x56\x57"), 0, DEBUGSTR("CShaderSystem::DrawElements"));
    m_arrOffset[GetTrueOffset(__CSHADERSYSTEM_PREPFORSHADERDRAW)] = new StaticOffset(__MATERIALSYSTEM, XorStr("\xE8????\x0F\xB6\x43\x5A"), 1, DEBUGSTR("CShaderSystem::PrepForShaderDraw"));
    m_arrOffset[GetTrueOffset(__THREADHANDLETOIDMAPS_COUNT)] = new StaticOffset(__TIER0, XorStr("\xFF\x05????\x66\x01?????\x5E\x75\x2B\x80"), 0, DEBUGSTR("g_nThreadHandleToIDMaps"));
    m_arrOffset[GetTrueOffset(__REMOVETHREADHANDLETOIDMAP)] = new StaticOffset(__TIER0, XorStr("\xE8????\x8A\xC3\x5B"), 1, DEBUGSTR("RemoveThreadHandleToIDMap"));
    m_arrOffset[GetTrueOffset(__INVALIDATEBONECACHE)] = new StaticOffset(__CLIENT, XorStr("\x80\x3D?????\x74\x16\xA1????\x48\xC7\x81"), 0, DEBUGSTR("Invalidate Bone Cache"));
    m_arrOffset[GetTrueOffset(__UNKNOWNRENDERVIEWFUNCTION2)] = new StaticOffset(__CLIENT, XorStr("\xE8????\x80\xBF?????\x74\x50\x8B\x0D????\x8B\x01\xFF\x90????"), 1, DEBUGSTR("ViewRender::RenderFinalOrSomethingToThatEffect"));
    m_arrOffset[GetTrueOffset(__SNP_SENDMESSAGE)] = new StaticOffset(__STEAMNETWORKING, XorStr("\x55\x8B\xEC\x83\xEC\x10\x53\x56\x57\x8B\xF9\x6A\x00"), 0, DEBUGSTR("CSteamNetworkConnectionBase::SNP_SendMessage"));
    m_arrOffset[GetTrueOffset(__SNP_GETNEXTTHINKTIME)] = new StaticOffset(__STEAMNETWORKING, XorStr("\xE8????\x3B\xD3\x7C\x0E"), 1, DEBUGSTR("CSteamNetworkConnectionBase::SNP_GetNextThinkTime"));
    m_arrOffset[GetTrueOffset(__NETSENDTO)] = new StaticOffset(__ENGINE, XorStr("\xE8????\x83\xC4\x08\xFF\x45\xF8"), 1, DEBUGSTR("NET_SendTo"));
    m_arrOffset[GetTrueOffset(__CSTEAMSOCKETMGR_ONP2PSESSIONREQUEST)] = new StaticOffset(__ENGINE, XorStr("\x55\x8B\xEC\x83\xE4\xF8\x51\x8B\x0D????\x56\x85\xC9\x0F\x84????\x8B\x01\xFF\x50\x34"), 0, DEBUGSTR("CSteamSocketMgr::OnP2PSessionRequest"));
    m_arrOffset[GetTrueOffset(__PROCESSCONNECTIONLESSPACKET)] = new StaticOffset(__ENGINE, XorStr("\x55\x8B\xEC\x83\xE4\xF0\x81\xEC????\x56\x57\x8B\xF9\x8B\x4D\x08"), 0, DEBUGSTR("CBaseClientState::ProcessConnectionlessPacket"));
    

    // 
    
    
    //

    

    //       auto boneCache = *(int**)(entity + 0x290C); FF B7 ? ? ? ? 52
    //       auto countBones = *(int*)(entity + 0x291B); 8B 87 ? ? ? ? 8B 4D 0C

    //
    //8B 06 83 B8 ? ? ? ? ? 7E 2EB

    // C_BaseAnimating::GetSequence (inlined member accessor by offset) direct reference: [actual address in first opcode] 81 CE ? ? ? ? 83 BF ? ? ? ? ? 
    // C_BaseAnimating::GetPrediction (inlined member accessor by offset (+746 05/10/2020)) direct reference: [actual address in first opcode] 7D 15 BA ? ? ? ?
    // C_BaseAnimating::m_iMostRecentBoneCounter (+9868 05/10/2020) direct reference: [actual address in first opcode] EB 05 F3 0F 10 45 ? 8B 87 ? ? ? ?
    // C_BaseAnimating::m_flLastBoneSetupTime (+10538 05/10/2020) direct reference: [actual address in first opcode] 8D 4F FC F3 0F 10 87 ? ? ? ?
    // C_BaseAnimating::m_pIK (+9836 05/10/2020) direct reference: [actual address in first opcode] 8D 47 FC 83 BF ? ? ? ? ?
    // C_BaseAnimating::m_p_StudioHdr (+10568 05/10/2020) sig: 8B B7 ? ? ? ? 89 74 24 20
    // g_iModelBoneCounter direct reference: [actual address in first opcode] 3B 05 ? ? ? ? 0F 84 ? ? ? ? 8B 47 FC 


    // (*(void (__stdcall **)(_DWORD *, char *, char *, int, int))(*(_DWORD *)(C_BaseAnimating - 4) + 820))(
    // StandardBlendingRules
    DEBUG_OUT_STR_VAR("Bone Accessor Offset : %d\n", *(short*)GetAddress(__BONEACCESSOR));
    DEBUG_OUT_STR_VAR(("Bone Accessor Hex Dump : %s\n"), hexStr((BYTE*)GetAddress(__BONEACCESSOR),8).c_str());
    DEBUG_OUT_STR_VAR(("C_BaseAnimating::CStudioHdr Offset : %d\n"), *(short*)GetAddress(__CBASEANIMATINGSTUDIOHDROFFSET));
    DEBUG_OUT_STR_VAR(("C_BaseAnimating::m_pIK Offset : %d\n"), *(short*)GetAddress(__CBASEANIMATINGIKCONTEXTOFFSET));
    DEBUG_OUT_STR_VAR(("C_BaseAnimating::m_EFlags Offset : %d\n"), *(short*)GetAddress(__CBASEANIMATINGEFLAGS)); // 228
    DEBUG_OUT_STR_VAR(("C_BaseAnimating::m_EntClientFlags Offset : %d\n"), *(byte*)GetAddress(__CBASEANIMATINGENTCLIENTFLAGS));
    DEBUG_OUT_STR_VAR(("C_BaseAnimating::m_BoneSetupLock Offset : %d\n"), *(short*)GetAddress(__CBASEANIMATINGSETUPBONELOCK));
    DEBUG_OUT_STR_VAR(("C_BaseAnimating::CachedBonesBase Offset : %d\n"), *(short*)GetAddress(__CACHEDBONEMATRIXOFFSET));

#if 0
    char* presetAddr = (char*)MemoryTools::FindPattern("gameoverlayrenderer", "\xFF\x15????\x8B\xF8\x85\xDB", "present") + (int)2;
    DEBUG_OUT_STR_VAR("Present Address From Our Scan : %d\n", presetAddr);
    DEBUG_OUT_STR_VAR("Present Address From Static Offset : %d\n", GetAddress(GetTrueOffset(__PRESENT)));
    DEBUG_OUT_STR_VAR("Present Offset Our Scan : %d\n", MemoryTools::CalculateOffset(__GAMEOVERLAYRENDERER, "\xFF\x15????\x8B\xF8\x85\xDB", 2));
    DEBUG_OUT_STR_VAR("Present Offset From Static Offset : %d\n", GetOffset(GetTrueOffset(__PRESENT)));
    DEBUG_OUT_STR_VAR("Offset Plus Base Address : %d\n", MemoryTools::CalculateOffset(__GAMEOVERLAYRENDERER, "\xFF\x15????\x8B\xF8\x85\xDB", 2) + g_pModules->GetModule(__GAMEOVERLAYRENDERER)->GetModuleBase());
    DEBUG_OUT_STR_VAR("Calculated Offset : %d\n", (DWORD)((char*)presetAddr - g_pModules->GetModule(__GAMEOVERLAYRENDERER)->GetModuleBase()));

    void* resertAddr = (char*)MemoryTools::FindPattern("gameoverlayrenderer", "\xC7\x45?????\xFF\x15????\x8B\xF8", "reset") + (int)9;
    DEBUG_OUT_STR_VAR("Reset Address From Our Scan : %d\n", resertAddr);
    DEBUG_OUT_STR_VAR("Reset Address From Static Offset : %d\n", GetAddress(GetTrueOffset(__RESET)));
    DEBUG_OUT_STR_VAR("Reset Offset Our Scan : %d\n", MemoryTools::CalculateOffset(__GAMEOVERLAYRENDERER, "\xC7\x45?????\xFF\x15????\x8B\xF8", 9));
    DEBUG_OUT_STR_VAR("Reset Offset From Static Offset : %d\n", GetOffset(GetTrueOffset(__RESET)));
    DEBUG_OUT_STR_VAR("Offset Plus Base Address : %d\n", MemoryTools::CalculateOffset(__GAMEOVERLAYRENDERER, "\xC7\x45?????\xFF\x15????\x8B\xF8", 9) + g_pModules->GetModuleBase(__GAMEOVERLAYRENDERER));
#endif


    
#else
#ifndef _LIVE
    WriteOffsets();
#else
    std::fstream fsOffsetFile;
    fsOffsetFile.open(XorStr("offsets.dat"), std::ios::in);
    if (!fsOffsetFile.is_open())
        QuickCrash;

    std::string str((std::istreambuf_iterator<char>(fsOffsetFile)),
        std::istreambuf_iterator<char>());
    ReadOffsets(str);
    fsOffsetFile.close();
#endif


#endif

    g_pThreadHandleToIDMaps = *(int**)((char*)GetAddress(__THREADHANDLETOIDMAPS_COUNT) + int(2));

    SaveData = reinterpret_cast<SaveDataFunc_t>(GetAddress(__SAVEDATA));
    RestoreData = reinterpret_cast<SaveDataFunc_t>(GetAddress(__RESTOREDATA));
    BaseEntityLockStudioHdr = reinterpret_cast<LockStudioHdrFunc_t>(__LOCKSTUDIOHDR);

    Globals::g_pClientState = **MemoryTools::FindPattern<CBaseClientState**>(__ENGINE, XorStr("\xA1????\x8B\x80????\xC3"), 1, DEBUGSTR("ClientState"));
    g_pLocalPlayer.init(*(Entity***)GetAddress(__LOCALPLAYER));


}
#include <string>
#include <streambuf>
#include <fstream>
#include <sstream>
#include <iomanip>
//#ifdef _DEBUG
#if 1
/* TODO: Encrypt!!!!!!!!!!! */
void Memory::WriteOffsets()
{
    json OffsetFile;

    OffsetFile["C"] = OFFSETMAX;
    for (int i = 0; i < OFFSETMAX; i++)
    {
        int nIndex = GetTrueOffset((Offset)i);
        StaticOffset* pOffset = m_arrOffset[nIndex];

        if (!pOffset) {
#ifndef _DEBUG
            QuickCrash;
#endif
            continue;
        }
        OffsetFile[std::to_string(nIndex).c_str()]["O"] = (uint32_t)pOffset->GetOffset();
        OffsetFile[std::to_string(nIndex).c_str()]["M"] = (uint32_t)pOffset->GetModule();
    }
    std::fstream fsOffsetFile(XorStr("offsets.dat"), std::ios::trunc | std::ios::out);
    std::string seralizedStr = OffsetFile.dump(4);
    fsOffsetFile.write(seralizedStr.c_str(), seralizedStr.size());
}
#endif
#ifndef _DEBUG
__declspec(noinline) bool Memory::ReadOffsets(std::string Data)
{
    json OffsetFile;
    try {
        OffsetFile = json::parse(Data);
    }
    catch (std::exception& e)
    {
#ifdef _LIVE
        QuickCrash;
#else
        OutputDebugStringA(e.what());
        return false;
#endif
    }


    OutputDebugStringA(OffsetFile.dump(4).c_str());

//#ifdef _LIVE
#if 1
    if (OffsetFile["C"] != OFFSETMAX)
        QuickCrash;
#endif

    for (int i = 0; i < OFFSETMAX; i++)
    {
        int nIndex = GetTrueOffset((Offset)i);

        if ((nIndex > OFFSETMAX) || (nIndex < 0))
            QuickCrash;

        Modules nModule = (Modules)-1;
        Offset nOffset = (Offset)-1;
        try {
            nModule = (Modules)(OffsetFile[std::to_string(nIndex).c_str()]["M"].get<unsigned int>());
            nOffset = (Offset)(OffsetFile[std::to_string(nIndex).c_str()]["O"].get<unsigned int>());

        } catch (std::exception& e)
        {
#ifdef _LIVE
            QuickCrash;
#else
            OutputDebugStringA(XorStr("Offset Read Error\n"));
            OutputDebugStringA(e.what());
            continue;
#endif
        }

        m_arrOffset[nIndex] = new StaticOffset(nModule, nOffset);

      



    }


    return true;
}
#endif



void GenerateOffsetsFile() {

   


}










// 55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6
// Standard Blending Rules