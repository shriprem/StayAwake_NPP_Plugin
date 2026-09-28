#pragma once

#include <ctime>
#include "../StayAwakeCore.h"
#include "../Utils.h"
#include "../NPP/DockingDlgInterface.h"

#define DEBUG_DISPLAY_IDLE_TICKS

constexpr auto BTN_TEXT_PAUSE = L"&Pause";
constexpr auto BTN_TEXT_RESUME = L"&Resume";

const wstring MIN_MAX_PERIOD = to_wstring(MIN_PERIOD) + L" and " + to_wstring(MAX_PERIOD);
const wstring INTERVAL_TOOLTIP = L"Number between " + MIN_MAX_PERIOD;
const wstring INTERVAL_WARNING = L"Please enter a value between " + MIN_MAX_PERIOD;
const LPCWSTR INTERVAL_WARN_TITLE = L"Timer Interval in seconds";


class StayAwakePanel : public DockingDlgInterface {
public:
   StayAwakePanel() :DockingDlgInterface(IDD_STAYAWAKE_DOCKPANEL) {};

   void initConfig();
   void initPanel();

   bool isPanelInitialized() const { return bPanelInitialized; }
   bool isPanelMounted() const { return panelMounted; }
   bool isTimerPaused();
   void stealthMode(bool active);

   virtual void display(bool toShow=true);
   void setParent(HWND parent2set);

   void showAboutDialog();

protected:
   bool bPanelInitialized{}, panelMounted{}, bSystemLocked{};
   UINT_PTR nTimerID{ 42 };

   UINT nRosterKeyCodes[LEN_ROSTER_KEYCODES]{};
   UINT nRosterLength{};

   UINT nIntervalMinSeconds{};
   UINT nIntervalMaxSeconds{};

   HWND hStealthMode{}, hPauseResume{};

   StayAwakeCore mAwakeCore{};

   virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

   void initRosterKeyCodes();
   void initAwakes();
   void pauseTimer();
   void killTimer();

   void showPausedInfo(bool both);
   void simulateAwakeKeyPress();
   void showSelectKeyCodesDialog();
   void onKillFocusIntervalMin();
   void onKillFocusIntervalMax();
   void onSetInterval();

   void onPanelResize(LPARAM lParam);
};

