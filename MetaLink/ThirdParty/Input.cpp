
#include "../BaseIncludes.h"
#include "Input.h"

bool CInput::OnWndProc(HWND hWnd, unsigned int msg, unsigned int param, unsigned int lParam)
{

	m_hLastWindowHandle = hWnd;

	switch (param)
	{
		case WM_ACTIVATEAPP:
			m_bActiveWindow = (bool)param;
			break;
		default:
			break;
	}

	switch (msg)
	{
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
	case WM_XBUTTONUP:
		return ProcessMouseMsg(msg, param);
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return ProcessKeyboardMsg(msg, param);
	default:
		return false;
	}

}

bool CInput::ProcessKeyboardMsg(unsigned int msg, unsigned int param)
{
	prevState[param] = keyState[param];
	switch (msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		keyState[param] = true;
		if (!keyStateDelayed[param])
		{
			double flTime = GetFloatTime();
			if ((m_arrdbLastCallTime[param] + m_dbKeyDelayTime) < flTime)
			{
				m_arrdbLastCallTime[param] = flTime;
				keyStateDelayed[param] = true;
			}
		}
		return true;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyState[param] = false;
		keyStateDelayed[param] = false;
		return true;

	default:
		return false;
	}
}

bool CInput::ProcessMouseMsg(unsigned int msg, unsigned int param)
{
	int key = 0;
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		prevState[VK_LBUTTON] = keyState[VK_LBUTTON];
		keyState[VK_LBUTTON] = true;

		{
			double flTime = GetFloatTime();
			if ((m_arrdbLastCallTime[param] + m_dbKeyDelayTime) < flTime)
			{
				m_arrdbLastCallTime[param] = flTime;
				keyStateDelayed[param] = true;
			}
		}

		return true;

	case WM_LBUTTONUP:
		prevState[VK_LBUTTON] = keyState[VK_LBUTTON];
		keyState[VK_LBUTTON] = false;
		keyStateDelayed[param] = false;
		return true;

	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		prevState[VK_RBUTTON] = keyState[VK_RBUTTON];
		keyState[VK_RBUTTON] = true;

		{
			double flTime = GetFloatTime();
			if ((m_arrdbLastCallTime[param] + m_dbKeyDelayTime) < flTime)
			{
				m_arrdbLastCallTime[param] = flTime;
				keyStateDelayed[param] = true;
			}
		}

		return true;

	case WM_RBUTTONUP:
		prevState[VK_RBUTTON] = keyState[VK_RBUTTON];
		keyState[VK_RBUTTON] = false;
		keyStateDelayed[param] = false;
		return true;

	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		prevState[VK_MBUTTON] = keyState[VK_MBUTTON];
		keyState[VK_MBUTTON] = true;
		return true;

	case WM_MBUTTONUP:
		prevState[VK_MBUTTON] = keyState[VK_MBUTTON];
		keyState[VK_MBUTTON] = false;
		return true;

	case WM_XBUTTONDOWN:
	case WM_XBUTTONDBLCLK:
		key = (HIWORD(param) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		prevState[key] = keyState[key];
		keyState[key] = true;
		return true;

	case WM_XBUTTONUP:
		key = (HIWORD(param) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		prevState[key] = keyState[key];
		keyState[key] = false;
		return true;

	default:
		return false;
	}
}


bool CInput::IsActiveWindow()
{
	return (GetForegroundWindow() == m_hLastWindowHandle);
}