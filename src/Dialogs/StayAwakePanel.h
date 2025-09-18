#pragma once

#include "../Utils.h"
#include "../NPP/DockingDlgInterface.h"

class StayAwakePanel : public DockingDlgInterface {
public:
   bool _newCmdLine{};

   StayAwakePanel() :DockingDlgInterface(IDD_STAYAWAKE_DOCKPANEL) {};

   void initPanel();

   virtual void display(bool toShow=true);
   void refreshDarkMode();
   void setParent(HWND parent2set);

   void showAboutDialog();

protected:
   bool bInitialized{};
   UINT_PTR nTimerID{};
   UINT nAwakeKeyCode{ 0 };
   UINT nTimerSeconds{ 240 };
   TCHAR sIniFilePath[MAX_PATH]{};

   HWND hKeyCodes{};

   virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

   void initTimer();
   void toggleScrollLock();
   void onKillfocusInterval();
   void onSetInterval();

   void onPanelResize(LPARAM lParam);
};

