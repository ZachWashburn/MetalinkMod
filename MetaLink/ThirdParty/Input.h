#pragma once
typedef struct HWND__* HWND;



// Credits to 3Ld4D for the original input class he made for Anteaus. Super clean, which is why I took it and added some stuff to it
// Very good code, all 3Ld's genius
struct CInput
{
	CInput()
	{
		memset(prevState, 0, sizeof(prevState));
		memset(keyState, 0, sizeof(keyState));
		memset(keyStateDelayed, 0, sizeof(keyStateDelayed));
	}
	bool OnWndProc(HWND hWnd, unsigned int msg, unsigned int param, unsigned int lParam);
	bool Down(int key) { return (keyState[key] && !m_bLocked); }
	bool Pressed(int key) { return (!prevState[key] && keyState[key] && !m_bLocked); }
	bool Released(int key) { return (prevState[key] && !keyState[key]); }
	bool Toggled(int key) { return (keyStateDelayed[key] && !m_bLocked); }
	void LockInput() { m_bLocked = true; }
	void UnLockInput() { m_bLocked = false; }
	bool IsActiveWindow();
	void OnNewFrame() { memset(keyStateDelayed, 0, sizeof(keyStateDelayed)); }
	HWND GetLastWindowHandle(){ return m_hLastWindowHandle; }
	void SetToggleKeyDelay(double flDelay) { m_dbKeyDelayTime = flDelay; }
private:
	bool ProcessMouseMsg(unsigned int msg, unsigned int param);
	bool ProcessKeyboardMsg(unsigned int msg, unsigned int param);
	bool prevState[256];
	bool keyState[256];
	bool keyStateDelayed[256];
	double m_arrdbLastCallTime[256];
	double m_dbKeyDelayTime = .2;
	bool m_bLocked = false;
	bool m_bActiveWindow = false;
	HWND m_hLastWindowHandle = NULL;
};

inline CInput* g_pInput = new CInput;