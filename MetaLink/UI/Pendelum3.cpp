#include "Pendelum3.h"


void Pendelum3::Pendelum3RenderInstance::Render()
{
	for (Pendelum3Window*& pWindow : m_vWindows)
	{
		pWindow->Render();
	}
}


void Pendelum3::Pendelum3RenderInstance::SetFocusWindow(Pendelum3Window* pSetWindow)
{
	bool bWasFound = false;
	int nIndex = 0;
	for (Pendelum3Window*& pWindow : m_vWindows)
	{
		if (pWindow == pSetWindow)
		{
			bWasFound = true;
			break;
		}
		nIndex++;
	}

	if (!pSetWindow)
	{
#ifdef _DEBUG
		VCON("[Pendelum3RenderInstance::SetFocusWindow] Attempted To Set An Unregistered Window As Focus Main\n");
#endif
		return;
	}

	if (nIndex == 0)
		return; // Already Focused

	for (int i = nIndex; i > 0; i--) // Overright Previous Unfocused window, shift everything back
	{
		m_vWindows.at(i) = m_vWindows.at(i - 1);
	}
	m_vWindows.at(0) = pSetWindow;
}



void Pendelum3::Pendelum3Window::AddMember(Pendelum3MenuItem* pItem)
{
	m_vMenuItems.push_back(pItem);
	pItem->SetContainer(this);
	pItem->UpdateSize();
	pItem->UpdateCoords();
}











