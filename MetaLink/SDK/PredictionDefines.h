#pragma once
#include "../FunctionTypeDeclares.h"


#define MULTIPLAYER_BACKUP 150

typedef enum
{
    PREDICTION_SIMULATION_RESULTS_ARRIVING_ON_SEND_FRAME = 0,
    PREDICTION_NORMAL,
} PREDICTION_REASON;


enum
{
    SLOT_ORIGINALDATA = -1,
};

inline SaveDataFunc_t SaveData;
inline SaveDataFunc_t RestoreData;