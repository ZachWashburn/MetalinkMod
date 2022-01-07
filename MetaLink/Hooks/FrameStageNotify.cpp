#include "FrameStageNotify.h"
#include "../Hooks.h"



void __fastcall hk_CHLClient_FrameStageNotify(CHLClient* _this, void*, ClientFrameStage_t nStage)
{
	switch (nStage)
	{
	case FRAME_UNDEFINED:		
		break;
	case FRAME_START:
		break;
	case FRAME_NET_UPDATE_START:
		break;
	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		break;
	case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		break;
	case FRAME_NET_UPDATE_END:
		break;
	case FRAME_RENDER_START:
		break;
	case FRAME_RENDER_END:
		break;
	case FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE:
		break;
	default:
		break;		
	};

	((FrameStageNotifyFunc_t)g_pHooks->client->GetOriginalAtIndex(37))(_this, nStage);
}