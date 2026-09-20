#pragma once

#include <ctime>
#include "../Utils.h"
#include "../NPP/DockingDlgInterface.h"

constexpr auto VK_UNASSIGNED_01 = 0x97;
constexpr auto VK_UNASSIGNED_10 = 0xE8;

constexpr auto PREF_INI_FILE = L"StayAwake.ini";
constexpr auto PREF_DEFAULTS = L"Defaults";
constexpr auto PREF_LEGACY_KEYCODE = L"AwakeKeyCode";
constexpr auto PREF_SELECTED_KEYCODES = L"SelectedKeyCodes";
constexpr auto PREF_AWAKE_PAUSED = L"AwakePaused";
constexpr auto PREF_INTERVAL_LEGACY = L"TimerIntervalInSeconds";
constexpr auto PREF_INTERVAL_MINIMUM = L"MinimumIntervalInSeconds";
constexpr auto PREF_INTERVAL_MAXIMUM = L"MaximumIntervalInSeconds";

constexpr auto BTN_TEXT_PAUSE = L"&Pause";
constexpr auto BTN_TEXT_RESUME = L"&Resume";

constexpr auto MIN_PERIOD{ 10 };
constexpr auto MAX_PERIOD{ 9990 };
constexpr auto DEF_PERIOD{ 240 };

constexpr auto LEN_ROSTER_KEYCODES{ 12 };
constexpr auto DEF_SELECTED_KEYCODES{ L"001111111111" };

const wstring MIN_MAX_PERIOD = to_wstring(MIN_PERIOD) + L" and " + to_wstring(MAX_PERIOD);
const wstring INTERVAL_TOOLTIP = L"Number between " + MIN_MAX_PERIOD;
const wstring INTERVAL_WARNING = L"Please enter a value between " + MIN_MAX_PERIOD;
const LPCWSTR INTERVAL_WARN_TITLE = L"Timer Interval in seconds";


class StayAwakePanel : public DockingDlgInterface {
public:
   bool _newCmdLine{};

   StayAwakePanel() :DockingDlgInterface(IDD_STAYAWAKE_DOCKPANEL) {};

   void initConfig();
   void initPanel();

   wstring getSelectedKeyCodes();
   bool checkSelectedKeyCodes(wstring sKeyCodes);
   bool saveSelectedKeyCodes(wstring sKeyCodes);

   bool isPanelInitialized() const { return bPanelInitialized; }
   bool isPanelMounted() const { return panelMounted; }
   bool isTimerPaused();
   void stealthMode(bool active);

   virtual void display(bool toShow=true);
   void setParent(HWND parent2set);

   void showAboutDialog();

protected:
   bool bPanelInitialized{}, panelMounted{};
   UINT_PTR nTimerID{ 42 };

   UINT nRosterKeyCodes[LEN_ROSTER_KEYCODES]{};
   UINT nRosterLength{};

   UINT nIntervalMinSeconds{};
   UINT nIntervalMaxSeconds{};
   TCHAR sIniFilePath[MAX_PATH]{};

   HWND hStealthMode{}, hPauseResume{};

   virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

   wstring getPreference(const wstring key, const wstring defaultVal) const;

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

