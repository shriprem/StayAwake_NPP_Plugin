#include "StayAwakePanel.h"
#include "AboutDialog.h"

extern HINSTANCE _gModule;
extern StayAwakePanel _awakePanel;
AboutDialog _aboutDlg;


constexpr auto PREF_INI_FILE = L"StayAwake.ini";
constexpr auto PREF_DEFAULTS = L"Defaults";
constexpr auto PREF_TIMER_INTERVAL = L"TimerIntervalInSeconds";

constexpr auto MIN_PERIOD{ 10 };
constexpr auto MAX_PERIOD{ 9990 };


INT_PTR CALLBACK StayAwakePanel::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) {
   switch (message) {
   case WM_COMMAND:
      switch LOWORD(wParam) {

      case IDC_STAYAWAKE_INTERVAL:
         if (HIWORD(wParam) == EN_KILLFOCUS)
            onKillfocusInterval();
         break;

      case IDC_STAYAWAKE_SET_INTERVAL:
         onSetInterval();
         break;

      case IDCLOSE:
         display(false);
         break;

      case IDC_STAYAWAKE_ABOUT_BUTTON:
         showAboutDialog();
         break;
      }

      break;

   case WM_TIMER:
      toggleScrollLock();
      break;

   case WM_LBUTTONDOWN:
   case WM_MBUTTONDOWN:
   case WM_RBUTTONDOWN:
      SetFocus(_hSelf);
      break;

   case WM_SETFOCUS:
      break;

   case WM_SIZE:
      onPanelResize(lParam);
      break;

   case WM_SHOWWINDOW:
      Utils::checkMenuItem(MI_GOTO_PANEL, wParam);
      break;

   default:
      return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
   }

   return FALSE;
}

void StayAwakePanel::initPanel() {
   nppMessage(NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)sIniFilePath);
   PathAppend(sIniFilePath, PREF_INI_FILE);

   nTimerSeconds = GetPrivateProfileInt(PREF_DEFAULTS, PREF_TIMER_INTERVAL, nTimerSeconds, sIniFilePath);
   nTimerSeconds = (nTimerSeconds < MIN_PERIOD || nTimerSeconds > MAX_PERIOD) ? 240 : nTimerSeconds; // default to 4 minutes if out of range

   SetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL, nTimerSeconds, FALSE);
   bInitialized = true;

   Utils::addTooltip(_hSelf, IDC_STAYAWAKE_INTERVAL, L"",
      wstring{ L"Number between " } + to_wstring(MIN_PERIOD) + L" and " + to_wstring(MAX_PERIOD), 3, TRUE);

   Utils::loadBitmap(_hSelf, IDC_STAYAWAKE_ABOUT_BUTTON, IDB_STAYAWAKE_ABOUT_BITMAP);
   Utils::addTooltip(_hSelf, IDC_STAYAWAKE_ABOUT_BUTTON, L"", ABOUT_DIALOG_TITLE, TRUE);

   initTimer();
}

void StayAwakePanel::display(bool toShow) {
   DockingDlgInterface::display(toShow);

   if (toShow) {
      SetFocus(GetDlgItem(_hSelf, IDC_STAYAWAKE_INTERVAL));
   }
   else {
      if (_aboutDlg.isCreated() && _aboutDlg.isVisible())
         _aboutDlg.display(FALSE);
   }
}

void StayAwakePanel::refreshDarkMode() {
   redraw();

   if (_aboutDlg.isCreated())
      _aboutDlg.refreshDarkMode();
}

void StayAwakePanel::setParent(HWND parent2set) {
   _hParent = parent2set;
}

void StayAwakePanel::showAboutDialog() {
   _aboutDlg.doDialog((HINSTANCE)_gModule);
}

void StayAwakePanel::initTimer() {
   toggleScrollLock();
   nTimerID = SetTimer(_hSelf, nTimerID, nTimerSeconds * 1000, NULL);
}

void StayAwakePanel::toggleScrollLock() {
   keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
   keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
   Sleep(10);
   keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
   keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

   SYSTEMTIME lastTime{};
   GetLocalTime(&lastTime);
   SetDlgItemText(_hSelf, IDC_STAYAWAKE_LAST_TOGGLE, Utils::formatSystemTime(lastTime, L"Scroll Lock was last toggled").c_str());

   SYSTEMTIME nextTime{};
   GetSystemTime(&nextTime);
   Utils::addSecondsToTime(nextTime, nTimerSeconds);
   SetDlgItemText(_hSelf, IDC_STAYAWAKE_NEXT_TOGGLE, Utils::formatSystemTime(nextTime, L"Scroll Lock will next toggle").c_str());
}

void StayAwakePanel::onKillfocusInterval() {
   if (!bInitialized) return;

   UINT nInterval{ GetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL, nullptr, FALSE) };

   if (nInterval < MIN_PERIOD || nInterval > MAX_PERIOD)
   {
      Utils::showEditBalloonTip(GetDlgItem(_hSelf, IDC_STAYAWAKE_INTERVAL), L"Timer Interval in seconds",
         (wstring{ L"Please enter a value between " } + to_wstring(MIN_PERIOD) + L" and " + to_wstring(MAX_PERIOD)).c_str());
      SetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL, nTimerSeconds, FALSE);
      return;
   }

   nTimerSeconds = nInterval;
}

void StayAwakePanel::onSetInterval() {
   onKillfocusInterval();
   WritePrivateProfileString(PREF_DEFAULTS, PREF_TIMER_INTERVAL, to_wstring(nTimerSeconds).c_str(), sIniFilePath);
   initTimer();
}

void StayAwakePanel::onPanelResize(LPARAM lParam) {
   // About button
   HWND hAboutBtn{ GetDlgItem(_hSelf, IDC_STAYAWAKE_ABOUT_BUTTON) };
   RECT rcAboutBtn;
   GetWindowRect(hAboutBtn, &rcAboutBtn);

   int aboutBtnWidth{ rcAboutBtn.right - rcAboutBtn.left };
   int aboutBtnHeight{ rcAboutBtn.bottom - rcAboutBtn.top };

   MoveWindow(hAboutBtn, (LOWORD(lParam) - aboutBtnWidth - 3), (HIWORD(lParam) - aboutBtnHeight - 3), aboutBtnWidth, aboutBtnHeight, TRUE);
}
