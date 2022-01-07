#pragma once
#include "../BaseIncludes.h"


class bf_write;
class old_bf_read;
namespace MessageUtils
{
	bool WriteProtobufToBuffer(google::protobuf::Message* Proto, bf_write* buffer, uint32_t MessageType);
	bool ReadProtobufFromBuffer(google::protobuf::Message* Proto, old_bf_read* buffer, uint32_t MessageType);
	int SendNetMessageToServer(google::protobuf::Message* Proto, uint32_t MessageType);


	// QuickHelpers for specific netmessages
	void SetNetChannelSlot(int nSlot);
	void SendServerNetStrimgCmd(std::string cmd);
	void SendSignonState(int nSpawnCount, int nSignon);
}
