#include "DataStore.h"
#include "SDK.h"


void DataStore::OnNewFrame()
{
	memcpy_s(&m_vmatWorldToScreen, sizeof(m_vmatWorldToScreen), &g_pInterfaces->m_pEngine->WorldToScreenMatrix(), sizeof(VMatrix));
	memcpy_s(&m_vmatWorldToView, sizeof(m_vmatWorldToView), &g_pInterfaces->m_pEngine->WorldToViewMatrix(), sizeof(VMatrix));
}
