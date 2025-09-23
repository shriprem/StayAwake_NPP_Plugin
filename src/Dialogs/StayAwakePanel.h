#pragma once

#include "../Utils.h"
#include "../NPP/DockingDlgInterface.h"

class StayAwakePanel : public DockingDlgInterface {
public:
   bool _newCmdLine{};

   StayAwakePanel() :DockingDlgInterface(IDD_STAYAWAKE_DOCKPANEL) {};

   void initConfig();
   void initPanel();

   bool isPanelInitialized() { return bPanelInitialized; }
   bool isTimerPaused();
   void initTimer();
   void killTimer();

   virtual void display(bool toShow=true);
   void setParent(HWND parent2set);

   void showAboutDialog();

protected:
   bool bPanelInitialized{};
   UINT_PTR nTimerID{ 42 };
   UINT nAwakeKeyCode{ 0 };
   UINT nTimerSeconds{ 240 };
   TCHAR sIniFilePath[MAX_PATH]{};

   HWND hKeyCodes{}, hPauseResume{};

   virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

   wstring getPreference(const wstring key, const wstring defaultVal) const;

   void pauseTimer();
   void showPausedInfo(bool both);
   void simulateAwakeKeyPress();
   void onKillfocusInterval();
   void onSetInterval();

   void onPanelResize(LPARAM lParam);
};

