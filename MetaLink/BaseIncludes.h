#pragma once
#include "PREPROCESSOR.h"
#include "ValveMemoryOverride.h"
#include "WindowsInclude.h"

#include <intrin.h>

#include <array>
#include <cstring>
#include <string_view>
#include <Psapi.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <array>

// Lets avoid the issues of Harpoon.......
#include <steam/steam_api.h>
#include <steam/steamtypes.h>
#include <steam/steamnetworkingtypes.h>
#include <steam/isteamgamecoordinator.h>
#include <steam/isteamfriends.h>
#include <steam/isteamnetworking.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/isteamnetworkingmessages.h>

#include "SDK/tier0/platform.h"

// You need to use Protobuf version 2.5.0. You can find this in the CSTRIKE15 Source Leak
#ifndef _DEBUG
#pragma comment(lib,"libprotobuf.lib")
#else
#pragma comment(lib,"libprotobuf.lib")
#endif

#include "Debug.h"
#include <d3d9.h>
#include <d3d9helper.h>
#include "d3dx9.h"
#pragma comment(lib, "d3dx9.lib")

#include "Utils/RebuiltWindowsAPI.h"

#include "ThirdParty/Input.h"
#include "ThirdParty/fnv.h"
#include "FunctionTypeDeclares.h"
#include "GlobalHelpers.h"
#include "PerformanceLogger.h"
#include "FileLog.h"


// Protobufs
#include "ThirdParty/Protobuf/Headers/cstrike15_gcmessages.pb.h"
#include "ThirdParty/Protobuf/Headers/cstrike15_usermessages.pb.h"
#include "ThirdParty/Protobuf/Headers/steammessages.pb.h"
#include "ThirdParty/Protobuf/Headers/steamdatagram_messages.pb.h"
#include "ThirdParty/Protobuf/Headers/steamdatagram_messages_auth.pb.h"
// #include "ThirdParty/Protobuf/Headers/steamdatagram_messages_sdr.pb.h" // TODO : Fix
#include "ThirdParty/Protobuf/Headers/steamdatagram_messages_auth.pb.h"
//#include "ThirdParty/Protobuf/Headers/steamnetworkingsockets_messages.pb.h" // TODO : Fix
#include "ThirdParty/Protobuf/Headers/netmessages.pb.h"
//#include "Globals.h"


// Global Values : 
inline bool g_bBlockP2PSessionRequests = true;
