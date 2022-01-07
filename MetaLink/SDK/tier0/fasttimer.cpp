//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#pragma once
#include "pch_tier0.h"

#include <stdio.h>
#include "tier0/fasttimer.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"

extern uint64 g_ClockSpeed;	// Clocks/sec
extern unsigned long g_dwClockSpeed;
extern double g_ClockSpeedMicrosecondsMultiplier;
extern double g_ClockSpeedMillisecondsMultiplier;
extern double g_ClockSpeedSecondsMultiplier;

// Constructor init the clock speed.
CClockSpeedInit g_ClockSpeedInit;
