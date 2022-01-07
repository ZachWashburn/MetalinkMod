#pragma once
#include "mathlib/mathlib.h"
#include "mathlib/vmatrix.h"

class DataStore
{
public:
	void OnNewFrame();
	const VMatrix& GetWorldToScreen() { return m_vmatWorldToScreen; }
private:
	VMatrix m_vmatWorldToScreen;
	VMatrix m_vmatWorldToView;
};

inline DataStore g_DataStore;
inline DataStore* g_pDataStore = &g_DataStore;