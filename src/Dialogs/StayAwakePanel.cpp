#include "StayAwakePanel.h"
#include "AboutDialog.h"

extern HINSTANCE _gModule;
AboutDialog _aboutDlg;


constexpr auto PREF_INI_FILE = L"StayAwake.ini";
constexpr auto PREF_DEFAULTS = L"Defaults";
constexpr auto PREF_AWAKE_KEYCODE = L"AwakeKeyCode";
constexpr auto PREF_AWAKE_PAUSED = L"AwakePaused";
constexpr auto PREF_TIMER_INTERVAL = L"TimerIntervalInSeconds";

constexpr auto BTN_TEXT_PAUSE = L"&Pause";
constexpr auto BTN_TEXT_RESUME = L"&Resume";

constexpr auto VK_UNASSIGNED_01 = 0x97;
constexpr auto VK_UNASSIGNED_10 = 0xE8;

constexpr auto MIN_PERIOD{ 10 };
constexpr auto MAX_PERIOD{ 9990 };


INT_PTR CALLBACK StayAwakePanel::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) {
   switch (message) {
   case WM_COMMAND:
      switch LOWORD(wParam) {

      case IDC_STAYAWAKE_KEY_LIST:
         switch HIWORD(wParam) {
         case CBN_SELCHANGE:
            nAwakeKeyCode = static_cast<int>(SendMessage(hKeyCodes, CB_GETCURSEL, 0, 0));
            nAwakeKeyCode %= 12;
            WritePrivateProfileString(PREF_DEFAULTS, PREF_AWAKE_KEYCODE, to_wstring(nAwakeKeyCode).c_str(), sIniFilePath);
            break;
         }
         break;

      case IDC_STAYAWAKE_INTERVAL:
         if (HIWORD(wParam) == EN_KILLFOCUS)
            onKillfocusInterval();
         break;

      case IDC_STAYAWAKE_SET_INTERVAL:
         onSetInterval();
         break;

      case IDC_STAYAWAKE_STEALTH_MODE:
         StayAwakeStealthMode();
         break;

      case IDC_STAYAWAKE_PAUSE_RESUME_BTN:
         if (isTimerPaused())
            initTimer();
         else
            pauseTimer();

         break;

      case IDCANCEL:
      case IDCLOSE:
         display(false);
         break;

      case IDC_STAYAWAKE_ABOUT_BUTTON:
         showAboutDialog();
         break;
      }

      break;

   case WM_TIMER:
      simulateAwakeKeyPress();
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
      Utils::checkMenuItem(MI_STAY_AWAKE_PANEL, wParam);
      break;

   default:
      return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
   }

   return FALSE;
}

void StayAwakePanel::initConfig() {
   NppMessage(NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)sIniFilePath);
   PathAppend(sIniFilePath, PREF_INI_FILE);

   nAwakeKeyCode = GetPrivateProfileInt(PREF_DEFAULTS, PREF_AWAKE_KEYCODE, nTimerSeconds, sIniFilePath);
   nAwakeKeyCode %= 12;

   nTimerSeconds = GetPrivateProfileInt(PREF_DEFAULTS, PREF_TIMER_INTERVAL, nTimerSeconds, sIniFilePath);
   nTimerSeconds = (nTimerSeconds < MIN_PERIOD || nTimerSeconds > MAX_PERIOD) ? 240 : nTimerSeconds; // default to 4 minutes if out of range
}

void StayAwakePanel::initPanel() {
   initConfig();

   hKeyCodes = GetDlgItem(_hSelf, IDC_STAYAWAKE_KEY_LIST);
   hStealthMode = GetDlgItem(_hSelf, IDC_STAYAWAKE_STEALTH_MODE);
   hPauseResume = GetDlgItem(_hSelf, IDC_STAYAWAKE_PAUSE_RESUME_BTN);

   // Init KeyCodes List
   SendMessage(hKeyCodes, CB_ADDSTRING, NULL, (LPARAM)L"Scroll Lock cycling");
   SendMessage(hKeyCodes, CB_ADDSTRING, NULL, (LPARAM)L"Volume Down & Up");

   for (int i{ 1 }; i <= 10; i++) {
      SendMessage(hKeyCodes, CB_ADDSTRING, NULL, (LPARAM)(L"Unassigned Key #" + to_wstring(i)).c_str());
   }

   SendMessage(hKeyCodes, CB_SETCURSEL, nAwakeKeyCode, NULL);

   // Init Timer Seconds
   SetDlgItemInt(_hSelf, IDC_STAYAWAKE_INTERVAL, nTimerSeconds, FALSE);

   Utils::addTooltip(_hSelf, IDC_STAYAWAKE_INTERVAL, L"",
      wstring{ L"Number between " } + to_wstring(MIN_PERIOD) + L" and " + to_wstring(MAX_PERIOD), 3, TRUE);

   SetWindowText(hPauseResume, isTimerPaused() ? BTN_TEXT_RESUME :BTN_TEXT_PAUSE);

   Utils::loadBitmap(_hSelf, IDC_STAYAWAKE_ABOUT_BUTTON, IDB_STAYAWAKE_ABOUT_BITMAP);
   Utils::addTooltip(_hSelf, IDC_STAYAWAKE_ABOUT_BUTTON, L"", ABOUT_DIALOG_TITLE, TRUE);

   if (isTimerPaused()) showPausedInfo(TRUE);
   bPanelInitialized = true;
}

void StayAwakePanel::display(bool toShow) {
   DockingDlgInterface::display(toShow);

   panelMounted = toShow;

   if (toShow) {
      if (!isTimerPaused()) initTimer();
      SetFocus(GetDlgItem(_hSelf, IDC_STAYAWAKE_INTERVAL));
   }
   else {
      if (_aboutDlg.isCreated() && _aboutDlg.isVisible())
         _aboutDlg.display(FALSE);
   }
}

void StayAwakePanel::setParent(HWND parent2set) {
   _hParent = parent2set;
}

void StayAwakePanel::showAboutDialog() {
   _aboutDlg.doDialog((HINSTANCE)_gModule);
}

wstring StayAwakePanel::getPreference(const wstring key, const wstring defaultVal) const {
   const int bufSize{ MAX_PATH };
   wstring ftBuf(bufSize, '\0');

   GetPrivateProfileString(PREF_DEFAULTS, key.c_str(), defaultVal.c_str(), ftBuf.data(), bufSize, sIniFilePath);

   return wstring{ ftBuf.c_str() };
}

bool StayAwakePanel::isTimerPaused() {
   return (getPreference(PREF_AWAKE_PAUSED, L"N") == L"Y");
}

void StayAwakePanel::showPausedInfo(bool both) {
   if (both)
      SetDlgItemText(_hSelf, IDC_STAYAWAKE_LAST_TOGGLE, L"Last StayAwake event:         PAUSED");

   SetDlgItemText(_hSelf, IDC_STAYAWAKE_NEXT_TOGGLE, L"Next StayAwake event:         PAUSED");
}

void StayAwakePanel::initTimer() {
   simulateAwakeKeyPress();
   nTimerID = SetTimer(_hSelf, nTimerID, nTimerSeconds * 1000, NULL);

   if (bPanelInitialized)
      SetWindowText(hPauseResume, BTN_TEXT_PAUSE);

   WritePrivateProfileString(PREF_DEFAULTS, PREF_AWAKE_PAUSED, L"N", sIniFilePath);
}

void StayAwakePanel::killTimer() {
   KillTimer(_hSelf, nTimerID);
}

void StayAwakePanel::stealthMode(bool active) {
   if (active) {
      if (!isTimerPaused()) initTimer();
   }
   else
      killTimer();

   CheckDlgButton(_hSelf, IDC_STAYAWAKE_STEALTH_MODE, active);
}

void StayAwakePanel::pauseTimer() {
   KillTimer(_hSelf, nTimerID);

   SetWindowText(hPauseResume, BTN_TEXT_RESUME);
   WritePrivateProfileString(PREF_DEFAULTS, PREF_AWAKE_PAUSED, L"Y", sIniFilePath);
   showPausedInfo(FALSE);
}

void StayAwakePanel::simulateAwakeKeyPress() {
   switch (nAwakeKeyCode) {
   case 1:
      keybd_event(VK_VOLUME_DOWN, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_VOLUME_DOWN, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      Sleep(10);
      keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      break;

   case 2:
   case 3:
   case 4:
   case 5:
   case 6:
   case 7:
   case 8:
   case 9:
   case 10:
   {
      BYTE keycode{ static_cast<BYTE>(VK_UNASSIGNED_01 + nAwakeKeyCode - 2) };
      keybd_event(keycode, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(keycode, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      break;
   }

   case 11:
      keybd_event(VK_UNASSIGNED_10, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_UNASSIGNED_10, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      break;

   default:
      keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      Sleep(10);
      keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_SCROLL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      break;

   }

   if (bPanelInitialized) {
      SYSTEMTIME lastTime{};
      GetLocalTime(&lastTime);
      SetDlgItemText(_hSelf, IDC_STAYAWAKE_LAST_TOGGLE, Utils::formatSystemTime(lastTime, L"Last StayAwake event").c_str());

      SYSTEMTIME nextTime{};
      GetSystemTime(&nextTime);
      Utils::addSecondsToTime(nextTime, nTimerSeconds);
      SetDlgItemText(_hSelf, IDC_STAYAWAKE_NEXT_TOGGLE, Utils::formatSystemTime(nextTime, L"Next StayAwake event").c_str());
   }
}

void StayAwakePanel::onKillfocusInterval() {
   if (!bPanelInitialized) return;

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
