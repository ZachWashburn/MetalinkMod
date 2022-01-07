#include "SendMessage.h"
#include "SDK.h"

// template <class PB_OBJECT_TYPE>
bool MessageUtils::WriteProtobufToBuffer(google::protobuf::Message* Proto, bf_write* buffer, uint32_t MessageType) {
	if (!Proto->IsInitialized())
	{
		VCON(XorStr("WriteToBuffer Message %s is not initialized! Probably missing required fields!"));
		return false;
	}

	int size = Proto->ByteSize();

	// If the write is byte aligned we can go direct
	if ((buffer->GetNumBitsWritten() % 8) == 0)
	{
		int sizeWithHeader = size + 1 + buffer->ByteSizeVarInt32(MessageType) + buffer->ByteSizeVarInt32(size);


		if (buffer->GetNumBytesLeft() >= sizeWithHeader)
		{
			buffer->WriteVarInt32(MessageType);
			buffer->WriteVarInt32(size);

			if (!Proto->SerializeWithCachedSizesToArray((google::protobuf::uint8*)buffer->GetData() + buffer->GetNumBytesWritten()))
			{
				return false;
			}

			// Tell the buffer we just splatted into it
			buffer->SeekToBit(buffer->GetNumBitsWritten() + (size * 8));
			return true;
		}

		// Won't fit
		return false;
	}

	// otherwise we have to do a temp allocation so we can write it all shifted

	void* serializeBuffer = alloca(size);

	if (!Proto->SerializeWithCachedSizesToArray((google::protobuf::uint8*)serializeBuffer))
	{
		VCON(XorStr("Message Write Failed!"));
		return false;
	}

	buffer->WriteVarInt32(MessageType);
	buffer->WriteVarInt32(size);
	return buffer->WriteBytes(serializeBuffer, size);
}

bool MessageUtils::ReadProtobufFromBuffer(google::protobuf::Message* Proto, old_bf_read* buffer, uint32_t MessageType) {
	int size = buffer->ReadVarInt32();
	if (size < 0 || size > NET_MAX_PAYLOAD)
	{
		return false;
	}

	// Check its valid
	if (size > buffer->GetNumBytesLeft())
	{
		return false;
	}

	// If the read buffer is byte aligned, we can parse right out of it
	if ((buffer->GetNumBitsRead() % 8) == 0)
	{
		bool parseResult = Proto->ParseFromArray(buffer->GetBasePointer() + buffer->GetNumBytesRead(), size);
		buffer->SeekRelative(size * 8);
		return parseResult;
	}

	// otherwise we have to do a temp allocation so we can read it all shifted
	VCON(XorStr("Warning: unaligned read of protobuf message %s (%d bytes)\n"), Proto->GetTypeName().c_str(), size);
	void* parseBuffer = stackalloc(size);
	if (!buffer->ReadBytes(parseBuffer, size))
	{
		return false;
	}

	if (!Proto->ParseFromArray(parseBuffer, size))
	{
		return false;
	}

	return true;
}

int MessageUtils::SendNetMessageToServer(google::protobuf::Message* Proto, uint32_t nMessageType)
{
	if (!Globals::g_pClientState->m_pNetChannel)
	{
#ifdef  _DEBUG
		VCON(XorStr("Unable To Send Message, no net channel!\n Message Details: \n%s"), Proto->DebugString().c_str());
#endif //  _DEBUG


		return -1;
	}

	char buffer[NET_MAX_PAYLOAD];
	bf_write msg(buffer, NET_MAX_PAYLOAD);
	WriteProtobufToBuffer(Proto, &msg, nMessageType);
	return Globals::g_pClientState->m_pNetChannel->SendDatagram(&msg);
}


void MessageUtils::SetNetChannelSlot(int nSlot)
{
	CNETMsg_SplitScreenUser user;
	user.set_slot(nSlot);
	SendNetMessageToServer(&user, net_SplitScreenUser);;
}



void MessageUtils::SendServerNetStrimgCmd(std::string cmd)
{
	CNETMsg_StringCmd stringcmd;
	stringcmd.set_command(cmd);
	SendNetMessageToServer(&stringcmd, net_StringCmd);;
}


void MessageUtils::SendSignonState(int nSpawnCount, int nSignon)
{
	CNETMsg_SignonState signon;
	signon.set_signon_state(nSignon);
	signon.set_spawn_count(nSpawnCount);
	SendNetMessageToServer(&signon, net_SignonState);;
}