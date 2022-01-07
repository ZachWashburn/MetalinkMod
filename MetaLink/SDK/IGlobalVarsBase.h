#pragma once

class IGlobalVarsBase
{
public:
	float				m_flRealtime;
	int					m_nFramecount;
	float				m_flAbsoluteframetime;
	float				m_flAbsoluteframestarttimestddev;
	float				m_flCurtime;
	float				m_flFrametime;
	int					m_nMaxClients;
	int					m_nTickcount;
	float				m_flIntervalPerTick;
	float				m_flInterpolationAmount;
	int					m_nSimTicksThisFrame;
	int					m_nNetwork_protocol;
	void*				m_pSaveData;
private:
	bool				m_bClient;
public:
	bool				m_bRemoteClient;
private:
	int					m_nTimestampNetworkingBase;
	int					m_nTimestampRandomizeWindow;
};
